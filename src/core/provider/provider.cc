#include"../../base/Y_Dragon.h" 
#include"provider.h"
#include"../../http/httpstatus.h"

namespace ws{
    bool Provider::IsFilename(char x) const {
        return !(x == '?' || x == '\\' || x == '/' || x == '*' ||
        x == '\"' || x == '\'' || x == '<' || x == '>' || x == '|');
    }

    int Provider::WriteHead(int ma, int mi, const HttpStatusCode& code){
        auto T = static_cast<int>(code);
        int ret = _Write_Loop_->swrite("HTTP/%d.%d %d %s\r\n",ma, mi, T, StatusCode_to_String(T));
        ret += _Write_Loop_->swrite("Server: Y_Dragon %s\r\n",Y_Dragon::Version());
        return ret;
    }

    int Provider::WriteDate(){
        time_t t = time(nullptr);
        char buf[100];
        strftime(buf, 70, "Date: %a, %d %b %Y %H: %M:%S GMT\r\n", gmtime(&t));
        return _Write_Loop_->swrite(buf);
    }

    int Provider::WriteItem(const char* key, const char* va){
        return _Write_Loop_->swrite(key, va);
    }

    int Provider::WriteConnection(){
        return _Write_Loop_->swrite("Connection", 
        _Request_->Return_Flag() == Keep_Alive ? "Keep-alive" : "Close");
    }

    int Provider::WriteCRLF(){
        return _Write_Loop_->swrite("/r/n", 2);
    } 

    int Provider::RegularProvide(long Content_Length, const char* Content_Type){
        int ret = WriteHead(_Request_->Return_Version_Ma(),_Request_->Return_Version_Mi(),
        _Request_->Return_Statuscode());
        ret += WriteDate();
        ret += WriteConnection();
        ret += WriteItem("Content-Type", Content_Type);
        ret += WriteItem("Content-Length", std::to_string(Content_Length).c_str());
        return ret;
    }

    const char* Provider::AutoAdapt() const{
        const char* Start = _Request_->Return_Uri().ReadPtr() +_Request_->Return_Uri().Length();
        const char* End = Start;

        const char* temp = _Request_->Return_Uri().ReadPtr();
        for(;Start != temp; --Start){
            if(*Start == '.'){
                break;
            }else if(! IsFilename(*Start)){
                End = Start;
            }
        }
        return Start == temp ? defaultMIME() : MIME(Start, std::distance(Start, End));
    }

    int Provider::RegularProvide(long Content_Length){
        RegularProvide(Content_Length, AutoAdapt());
    }

    //TODU 类型在csdn收藏中 需要正则去处理文件
    const char* Provider::MIME(const char* type, ptrdiff_t len) const{

    }

    int Provider::ProvideError(){
        static constexpr const char temp[] = "<html><head><title>Transgate/HTTP Error</title></head>";
        size_t len = strlen(temp);
        _Request_->Set_StatusCode(HSCBadRequest);
        int ret = RegularProvide(len - 1);
        ret += WriteCRLF();
        ret += _Write_Loop_->swrite(temp, len);
        _Write_Loop_->AddSend(ret);
        return ret;
    }

}