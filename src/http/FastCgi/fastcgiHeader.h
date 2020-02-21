#ifndef FASTCGIHRADER_H_
#define FASTCGIHRADER_H_

//监听sock
#define FCGI_LISTENSOCK_FILENO 0

typedef struct {
    unsigned char version;              //版本 
    unsigned char type;                 //操作类型
    unsigned char requestIdB1;          //请求id
    unsigned char requestIdB0;          
    unsigned char contentLengthB1;      //内容长度
    unsigned char contentLengthB0;
    unsigned char paddingLength;        //填充字节的长度
    unsigned char reserved;             //保留字节
}FCGI_Header;

//允许传送的最大数据65536
#define FCGI_MAX_LENGTH 0xffff

//上述FCGI_HEADER长度
#define FCGI_HEADER_LEN  8


//FCGI的版本
#define FCGI_VERSION_1           1

//上述FCGI_Header中type的具体值
enum FCGI_Type {
    FCGI_BEGIN_REQUEST     = 1, //表示一次请求的开始
    FCGI_ABORT_REQUEST     = 2, //表示终止一次请求
    FCGI_END_REQUEST       = 3, //请求已被处理完毕  
    FCGI_PARAMS            = 4, //表示一个向CGI程序传递的环境变量 
    FCGI_STDIN             = 5, //表示向CGI程序传递的标准输入  
    FCGI_STDOUT            = 6, //表示CGI程序的标准输出
    FCGI_STDERR            = 7, //表示CGI程序的标准错误输出  
    FCGI_DATA              = 8, //向CGI程序传递的额外数据
    FCGI_GET_VALUES        = 9, //向FastCGI程序询问一些环境变量
    FCGI_GET_VALUES_RESULT = 10,//询问环境变量的结果
    FCGI_UNKNOWN_TYPE      = 11 //未知类型 可能用作拓展 
};

//空的请求ID
#define FCGI_NULL_REQUEST_ID     0

typedef struct 
{
    unsigned char roleB1;       //web服务器所期望php-fpm扮演的角色，具体取值下面有
    unsigned char roleB0;
    unsigned char flags;        //确定php-fpm处理完一次请求之后是否关闭
    unsigned char reserved[5];  //保留字段
}FCGI_BeginRequestBody;

typedef struct 
{
    FCGI_Header header;         //webserver请求php-fpm的请求头
    FCGI_BeginRequestBody body;//请求体

}FCGI_BeginRequestRecord;
 

//webserver期望应用扮演的角色
#define FCGI_KEEP_CONN  1 //如果为0则处理完请求应用就关闭，否则不关闭

enum FCGI_Role {
  FCGI_RESPONDER  = 1,  // 响应器
  FCGI_AUTHORIZER = 2,  // 认证器
  FCGI_FILTER     = 3   // 过滤器
};


typedef struct 
{
    unsigned char appStatusB3;      //结束状态，0为正常
    unsigned char appStatusB2;
    unsigned char appStatusB1;
    unsigned char appStatusB0;
    unsigned char protocolStatus;   //协议状态
    unsigned char reserved[3];
}FCGI_EndRequestBody;

typedef struct 
{
    FCGI_Header header;         //结束头
    FCGI_EndRequestBody body;   //结束体
}FCGI_EndRequestRecord;


//几种结束状态
enum FCGI_ProtocolStatus {
  FCGI_REQUEST_COMPLETE = 0,  // 请求正常完成
  FCGI_CANT_MPX_CONN    = 1,  // FastCGI服务器不支持并发处理，请求已被拒绝
  FCGI_OVERLOADED       = 2,  // FastCGI服务器耗尽了资源或达到限制，请求已被拒绝
  FCGI_UNKNOWN_ROLE     = 3   // FastCGI不支持指定的role，请求已被拒绝
};

#define FCGI_MAX_CONNS  "FCGI_MAX_CONNS"    //可接受的并发传输线路的最大值

#define FCGI_MAX_REQS   "FCGI_MAX_REQS"     //可接受并发请求的最大值

#define FCGI_MPXS_CONNS "FCGI_MPXS_CONNS"   //是否多路复用，其状态值也不同

typedef struct 
{
    unsigned char type;     
    unsigned char reserved[7];
}FCGI_UnknownTypeBody;

typedef struct 
{
    FCGI_Header header;
    FCGI_UnknownTypeBody body;
}FCGI_UnknownTypeRecord;

#endif //FASTCGIHRADER_H_