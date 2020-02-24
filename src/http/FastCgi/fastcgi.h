/**
 * Copyright lizhaolong(https://github.com/Super-long)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef FASTCGI_H_
#define FASTCGI_H_

#include "fastcgiHeader.h"
#include "../../net/socket.h"

namespace ws{

class FastCgi{
private:
int requestId_ = 0; //TODO 这个的分配很重要
int HtmlFlag = 0;
Socket socket_;

static const int CONTENT_BUFFER_LEN = 1024;
public:
    FastCgi() : socket_(::socket(AF_INET,SOCK_STREAM,0)){}
    void start(const std::string& path, const std::string& data);
    int ReturnSocketFd() const noexcept{return socket_.fd();}
    std::string ReadContent(); //从套接字读取消息

private:
    //生成头部
    void SetRequestID(int ID) noexcept {requestId_ = ID;}
    
    void Conection(); //连接php-fpm服务器
    bool StartRequest(); //开始一次请求
    bool EndRequest();
    FCGI_Header CreateHeader(int type, int request, int contentLength, int paddingLength = 0);
    FCGI_BeginRequestBody CreateBeginRequestBody(int role,int keepConnection);
    void SendContent(const std::string& tag, const std::string& value); //设置消息体
    //生成PARAMS的name-value body
    void CreateContentValue(const std::string& name,int nameLen,
                            const std::string& value,int valueLen,
                            unsigned char* ContentBuffPtr, int* ContentLen);
    void SendRequest(const std::string& data, size_t len);
    void GetContent(char* data); //还要再改
    char* FindStart(char* data);
};

}

#endif //FASTCGI_H_