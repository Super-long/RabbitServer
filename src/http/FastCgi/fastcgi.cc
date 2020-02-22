#include "fastcgi.h"
#include <netinet/in.h> //sockaddr_in
#include <arpa/inet.h> //inet_addr
//#include <sys/types.h>
#include <sys/socket.h> //connect
#include <iostream> //cout endl
#include <assert.h> //assert
#include <algorithm> //for_each

namespace ws{

void 
FastCgi::Conection(){
    struct sockaddr_in ServerAddress;
    memset(&ServerAddress, 0, sizeof ServerAddress);
    constexpr char IP[] = "127.0.0.1";
    assert(socket_.fd() > 0);

    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = inet_addr(IP);
    ServerAddress.sin_port = htons(9000);

    int ret = ::connect(socket_.fd(), (struct sockaddr *)&ServerAddress, sizeof ServerAddress);
    if(ret < 0) {
        std::cout << __FILE__ << "      " << __LINE__ << std::endl ;
    }
    assert(ret >= 0);
}

FCGI_Header 
FastCgi::CreateHeader(int type, int request, int contentLength, int paddingLength){
    FCGI_Header header;
    header.version = FCGI_VERSION_1; //版本一般设置为1
    header.type    = (unsigned char)type; //设置请求类型
        
    //用两个字段保存请求ID
    header.requestIdB1 = (unsigned char)((request >> 8) & 0xff); //高八位
    header.requestIdB0 = (unsigned char)(request & 0xff); //第八位

    //用两个字段保存内容长度
    header.contentLengthB1 = (unsigned char)((contentLength >> 8) & 0xff); //高八位
    header.contentLengthB0 = (unsigned char)(contentLength & 0xff); //低八位
    
    header.paddingLength = (unsigned char)paddingLength; //填充字节的长度 保证字节对齐 TODO 查下
    header.reserved = 0; //保留字节赋为0 
    return header; //RVO
}

FCGI_BeginRequestBody
FastCgi::CreateBeginRequestBody(int role,int keepConnection){
    FCGI_BeginRequestBody body;

    body.roleB1 = (unsigned char)((role >> 8) & 0xff);//两个字节表示期望php-fpm扮演的角色 一般为响应者
    body.roleB0 = (unsigned char)(role & 0xff);

    body.flags = (unsigned char)((keepConnection) ? FCGI_KEEP_CONN : 0);//大于0常连接，否则短连接

    memset(&body.reserved, 0 ,sizeof body.reserved);
    return body;
}

bool
FastCgi::StartRequest(){
    FCGI_BeginRequestRecord FirstRecord;

    FirstRecord.header = CreateHeader(FCGI_BEGIN_REQUEST, requestId_, sizeof FirstRecord.body);
    FirstRecord.body = CreateBeginRequestBody(FCGI_RESPONDER, 0); //当做响应器 发出消息执行后返回

    int ret = ::write(socket_.fd(), (char *)&FirstRecord, sizeof FirstRecord);
    assert(ret == sizeof(FirstRecord));
    return true;
}

void
FastCgi::CreateContentValue(const std::string& name, int nameLen, //ContentBuffPtr为要写入的地方
    const std::string& value, int valueLen, unsigned char* ContentBuffPtr, int* ContentLen){
    
    unsigned char* StartSpot_Ptr = ContentBuffPtr;

    if(nameLen < 128){ //如果nameLen长度小于128字节
        *ContentBuffPtr++ = (unsigned char)nameLen; //nameLen用一个字节保存
    }else{ //nameLen用4个字节保存
        *ContentBuffPtr++ = (unsigned char)((nameLen >> 24) | 0x80); //感觉后面并没有什么意义
        *ContentBuffPtr++ = (unsigned char)(nameLen >> 16); //自动去除高位
        *ContentBuffPtr++ = (unsigned char)(nameLen >> 8);
        *ContentBuffPtr++ = (unsigned char)nameLen;
    }
    if(valueLen < 128){
        *ContentBuffPtr++ = (unsigned char)valueLen;
    }else{
        *ContentBuffPtr++ = (unsigned char)((valueLen >> 24) | 0x80);
        *ContentBuffPtr++ = (unsigned char)(valueLen >> 16);
        *ContentBuffPtr++ = (unsigned char)(valueLen >> 8);
        *ContentBuffPtr++ = (unsigned char)valueLen;
    }
    std::for_each(name.cbegin(), name.cend(), [&](char ch){*ContentBuffPtr++ = ch;});
    std::for_each(value.cbegin(), value.cend(), [&](char ch){*ContentBuffPtr++ = ch;});
    *ContentLen = std::distance(StartSpot_Ptr, ContentBuffPtr);
}

void 
FastCgi::SendContent(const std::string& tag, const std::string& value){
    unsigned char bodyBuffer[CONTENT_BUFFER_LEN];
    memset(bodyBuffer, 0 , sizeof bodyBuffer);

    int ContentLength = 0; 
    CreateContentValue(tag, tag.size(), value, value.size(), bodyBuffer, &ContentLength); //得到发送的内容与长度

    FCGI_Header ContentHeader(CreateHeader(FCGI_PARAMS, requestId_, ContentLength));

    int AllContentLength = ContentLength + FCGI_HEADER_LEN;
    char Content[AllContentLength];
    //memset(Content, 0, AllContentLength*sizeof(char));

    memcpy(Content, (char*)&ContentHeader, FCGI_HEADER_LEN);
    memcpy(Content + FCGI_HEADER_LEN, bodyBuffer, ContentLength);

    int ret = ::write(socket_.fd(), Content, AllContentLength);
    assert(ret == AllContentLength);
}

bool 
FastCgi::EndRequest(){
    FCGI_Header EndHeader(CreateHeader(FCGI_PARAMS, requestId_, 0));

    int ret = ::write(socket_.fd(), (char *)&EndHeader, FCGI_HEADER_LEN);
    assert(ret == FCGI_HEADER_LEN);

    return true;
}

void
FastCgi::SendRequest(const std::string& data, size_t len){
    FCGI_Header IntervalHead(CreateHeader(FCGI_STDIN, requestId_, len)); //发送的输入
    int ret_IntervalHead = ::send(socket_.fd(), (char*)&IntervalHead, FCGI_HEADER_LEN, 0);
    int ret_Data = ::send(socket_.fd(), data.c_str(), len, 0);
    FCGI_Header EndHeader(CreateHeader(FCGI_STDIN, requestId_, 0));
    int ret_EndHead = ::send(socket_.fd(), (char*)&EndHeader, FCGI_HEADER_LEN, 0);
    //网络上的问题目前不太好解决, 消息发送不完全的话就退出
}

std::string
FastCgi::ReadContent(){
    FCGI_Header parsering;
    char Content[CONTENT_BUFFER_LEN];
    memset(Content, 0, sizeof Content);
    int ContentLength = 0;
    int ret = 0;
    char Trash[128];
    memset(Trash, 0 , sizeof Trash);

    while(::read(socket_.fd(), &parsering, FCGI_HEADER_LEN) > 0){
        if(parsering.type == FCGI_STDOUT){ //正常输出
            ContentLength = (parsering.contentLengthB1 << 8) + (parsering.contentLengthB0);//得到内容长度
            memset(Content, 0, CONTENT_BUFFER_LEN);
            //读取获取的内容
            ret = ::read(socket_.fd(), Content, ContentLength); //得到内容
            assert(ret == ContentLength); 

            GetContent(Content); //Content中为HTML格式 TODO 其实不是

            if(parsering.paddingLength > 0){
                ret = read(socket_.fd(), Trash, parsering.paddingLength);
                assert(ret == parsering.paddingLength);
            }
        }else if(parsering.type == FCGI_STDERR){ //错误输出
            ContentLength = (parsering.contentLengthB1 << 8) + (parsering.contentLengthB0);
            memset(Content, 0, CONTENT_BUFFER_LEN);
            
            ret = ::read(socket_.fd(), Content, ContentLength); //得到内容
            assert(ret == ContentLength); 

            if(parsering.paddingLength > 0){
                ret = read(socket_.fd(), Trash, parsering.paddingLength);
                assert(ret == parsering.paddingLength);
            }    
        }else if(parsering.type == FCGI_STDERR){ //错误输出
            FCGI_EndRequestBody endRequest;

            ret = read(socket_.fd(), &endRequest, sizeof endRequest);
            assert(ret == sizeof(endRequest));
            std::cout << "内容接收至结尾.\n";
        }
    }
    return Content;
}

void
FastCgi::GetContent(char* data){
    char *pt;   //保存html内容开始位置

    if(1 == HtmlFlag){   //读取到的content是html内容
    //TODO 这个值得搞一手
    }else{
        if((pt = FindStart(data)) != NULL){
            HtmlFlag = 1;
            for(char* i = pt;*i != '\0';i++){}        
        }          
    }  
}

char* FastCgi::FindStart(char* data){
    for(;*data != '\0';data++){
        if(*data == '<')
            return data;
    }
    return NULL;
}


void //传入要请求的路径与输入
FastCgi::start(const std::string& path, const std::string& data){ 
    ++requestId_;
    //连接服务器php-fpm
    Conection() ;
    StartRequest() ; //发送初始请求头和请求体 
    //FCGI_BEGIN_REQUEST

    //设置method 为post
    //设置script_name变量为php资源路径
    //设置content-type　文本格式
    //设置content-length消息长度
    //将要操作的数据传过去
    char Length[32] ;
    sprintf(Length, "%d", data.size()) ;
    //先发送一个头部 然后在发送消息体 消息体由一个FCGI_Header和消息体在一起.
    //FCGI_PARAMS
    SendContent("SCRIPT_FILENAME", path.c_str());
    SendContent("REQUEST_METHOD", "POST") ;
    SendContent("CONTENT_LENGTH", Length) ;
    SendContent("CONTENT_TYPE", "application/x-www-form-urlencoded") ;

    EndRequest() ; //请求头结束
    //FCGI_PARAMS   
    //结束发送 发送
    SendRequest(data.c_str(), data.size());
}

}