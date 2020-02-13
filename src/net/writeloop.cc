#include"writeloop.h"
#include<cstdarg>
#include<sys/socket.h>

namespace ws{
    int WriteLoop::swrite(const char* format, ...){
        va_list va;
        va_start(va, format);
        int ret = User_Buffer_->SWrite(format, va);
        va_end(va);
        write(ret);
        return ret;
    }

    bool WriteLoop::Send(int length){
        if(length < 1) throw std::invalid_argument("'WriteLoop::Send' error parameter.");
        int sent_ = 0;
        for(int ans;length - sent_ && (ans = static_cast<int>(send(fd_, User_Buffer_->ReadPtr(),length - sent_, 0))) > 0;){
            sent_ += ans;
        }
        User_Buffer_->read(sent_);
        int Remaining = length - sent_; 
        if(Remaining > 0){
            InsertSend(Remaining);
            return false;
        }
        return true;
    }

    bool WriteLoop::SendFile(std::shared_ptr<FileReader> ptr){
        ssize_t len = 0;
        while(len = ptr->SendFile(fd_) && len > 0){}
        if(ptr->Send_End()){
            InsertSendFile(ptr);
            return false;
        }
        return true;
    }

    bool WriteLoop::DoFirst(){
        if(!Que.empty()) {
            auto Fun = Que.front();
            Que.pop_front();
            return Fun();
        }
        return false;
    }

}