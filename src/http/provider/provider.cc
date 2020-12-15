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

#include "../../base/config.h" 
#include "provider.h"
#include "../../http/httpstatus.h"
#include "mime.cc"

#ifndef __GNUC__

#define  __attribute__(x)  /*NOTHING*/
    
#endif

// 这个文件中的函数大多数在每次请求中只会调用一次，所以用不上hot

namespace ws{
    bool constexpr __attribute__((pure)) Provider::IsFilename(char x){
        return !(x == '?' || x == '\\' || x == '/' || x == '*' ||
        x == '\"' || x == '\'' || x == '<' || x == '>' || x == '|');
    }

    int Provider::WriteHead(int ma, int mi, const HttpStatusCode& code){
        auto T = static_cast<int>(code);
        int ret = _Write_Loop_->swrite("HTTP/%d.%d %d %s\r\n",ma, mi, T, StatusCode_to_String(T));
        ret += _Write_Loop_->swrite("Server: RabbitServe(Y_Dragon) %s\r\n",Y_Dragon::Version());
        return ret;
    }

    int Provider::WriteDate(){
        time_t t = time(nullptr);
        char buf[70];
        strftime(buf, 70, "Date: %a, %d %b %Y %H: %M:%S GMT\r\n", gmtime(&t));
        return _Write_Loop_->swrite(buf);
    }

    int __attribute__((hot)) Provider::WriteItem(const char* key, const char* va){
        return _Write_Loop_->swrite(key, va);
    }

    int Provider::WriteConnection(){
        return _Write_Loop_->swrite("Connection: %s", 
        _Request_->Return_Flag() == Keep_Alive ? "Keep-alive\n" : "Close\n");
    }

    int Provider::WriteCRLF(){
        return _Write_Loop_->swrite("/r/n", 2);
    }

    /**
     * @return: 写入缓冲区字节数；
     * @notes:  默认的响应报文部分;
    */
    int Provider::RegularProvide(long Content_Length, const char* Content_Type){
        int ret = WriteHead(_Request_->Return_Version_Ma(), _Request_->Return_Version_Mi(), _Request_->Return_Statuscode());
        ret += WriteDate();
        ret += WriteConnection();
        ret += WriteItem("Content-Type: %s", Content_Type);     // 类型未完成
        ret += WriteItem("\nContent-Length: %s", std::to_string(Content_Length).c_str());
        ret += WriteItem("\nContent-Language: %s", "en-US");    // 语言这一项后面再改吧，目前默认en-US
        return ret;
    }

    std::string Provider::AutoAdapt() const {
        const char* Start = _Request_->Return_Uri().ReadPtr() +_Request_->Return_Uri().Length();
        const char* End = Start;

        const char* temp = _Request_->Return_Uri().ReadPtr();
        for(;Start != temp; --Start){
            if(*Start == '.'){
                break;
            }else if(!IsFilename(*Start)){
                End = Start;
            }
        }

        return Start == temp ? defaultMIME() : MIME(Start, std::distance(Start , End));
    }

    // 供外界调用的接口
    int Provider::RegularProvide(long Content_Length){
        return RegularProvide(Content_Length, AutoAdapt().c_str());
    }

    // 这段代码的确很挫，我知道你知道，先不要骂我2B，请先接着往下看;
    static FastFindMIMEMatcher FindMIME;   // 搞个全局变量，设计其实有点问题，放到provider其实更合适，但是我不想改了；

    std::string Provider::MIME(const char* type, ptrdiff_t len) const {
        auto res = FindMIME.get(std::string(type, len));
        return res == std::string("nullptr") ? nullptr : res;   //和女朋友意见不一致的时候听她的，意见一致的时候听我的。啊，不小心忘了我没有女朋友；
    }

    constexpr size_t Getstrlen(const char* str){
        if(str == nullptr) return 0;
        size_t len = 0;
        char ch = *str;
        while(ch != '\0'){
            len++;
            str++;
            ch = *str;
        }
        return len;
    }

    int Provider::ProvideError(){
        static constexpr const char temp[] = "<html><head><title>RabbitServer/HTTP Error</title></head>";
        auto len = Getstrlen(temp);
        //size_t len = strlen(temp);
        _Request_->Set_StatusCode(HSCBadRequest);
        int ret = RegularProvide(len - 1);
        ret += WriteCRLF();
        ret += _Write_Loop_->swrite(temp, len);
        _Write_Loop_->AddSend(ret);
        return ret;
    }

}