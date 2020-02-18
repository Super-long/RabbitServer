#include "writeloop.h"
#include <cstdarg>
#include <sys/socket.h>

namespace ws{
    int WriteLoop::swrite(const char* format, ...){
        va_list va;
        va_start(va, format);
        int ret = User_Buffer_->SWrite(format, va);
        va_end(va);
        write(ret);
        return ret;
    }

    WriteLoop::COMPLETETYPE WriteLoop::Send(int length){
        if(length < 1) throw std::invalid_argument("'WriteLoop::Send' error parameter.");
        int sent_ = 0;
        for(int ans;length - sent_ && (ans = static_cast<int>(send(fd_, User_Buffer_->ReadPtr(),length - sent_, 0))) > 0;){
            sent_ += ans;
        }
        User_Buffer_->read(sent_);
        int Remaining = length - sent_; 
        bool faultError = false;
        if (errno != EWOULDBLOCK){
            if (errno == EPIPE || errno == ECONNRESET){//服务端收到RST报文后 write出现EPIPE recv出现ECONNRESET
                faultError = true;
            }
        }
        if (!faultError && Remaining > 0){//write failture.
            InsertSend(Remaining);
            return IMCOMPLETE;
        }
        return COMPLETE;
    }

    WriteLoop::COMPLETETYPE WriteLoop::SendFile(std::shared_ptr<FileReader> ptr){
        ssize_t len = 0;
        while(len = ptr->SendFile(fd_) && len > 0){}
        if(!ptr->Send_End()){
            InsertSendFile(ptr);
            return IMCOMPLETE;
        }
        return COMPLETE;
    }

    WriteLoop::COMPLETETYPE WriteLoop::DoFirst(){
        if(!Que.empty()) {
            auto Fun = Que.front();
            Que.pop_front();
            return Fun();
        }
        return EMPTY;
    }

    WriteLoop::COMPLETETYPE WriteLoop::DoAll(){
        while(1){
            auto CompleteType = DoFirst();
            if(CompleteType == COMPLETE) continue;
            else return CompleteType;
        }
    }

}