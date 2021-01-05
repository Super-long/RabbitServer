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

#include "writeloop.h"

#include <cstdarg>
#include <sys/socket.h>
#include <netinet/tcp.h> 

#ifndef __GNUC__

#define  __attribute__(x)  /*NOTHING*/
    
#endif

/*
                                   .. .vr       
                                 qBMBBBMBMY     
                                8BBBBBOBMBMv    
                              iMBMM5vOY:BMBBv        
              .r,             OBM;   .: rBBBBBY     
              vUL             7BB   .;7. LBMMBBM.   
             .@Wwz.           :uvir .i:.iLMOMOBM..  
              vv::r;             iY. ...rv,@arqiao. 
               Li. i:             v:.::::7vOBBMBL.. 
               ,i7: vSUi,         :M7.:.,:u08OP. .  
                 .N2k5u1ju7,..     BMGiiL7   ,i,i.  
                  :rLjFYjvjLY7r::.  ;v  vr... rE8q;.:,, 
                 751jSLXPFu5uU@guohezou.,1vjY2E8@Yizero.    
                 BB:FMu rkM8Eq0PFjF15FZ0Xu15F25uuLuu25Gi.   
               ivSvvXL    :v58ZOGZXF2UUkFSFkU1u125uUJUUZ,   
             :@kevensun.      ,iY20GOXSUXkSuS2F5XXkUX5SEv.  
         .:i0BMBMBBOOBMUi;,        ,;8PkFP5NkPXkFqPEqqkZu.  
       .rqMqBBMOMMBMBBBM .           @kexianli.S11kFSU5q5   
     .7BBOi1L1MM8BBBOMBB..,          8kqS52XkkU1Uqkk1kUEJ   
     .;MBZ;iiMBMBMMOBBBu ,           1OkS1F1X5kPP112F51kU   
       .rPY  OMBMBBBMBB2 ,.          rME5SSSFk1XPqFNkSUPZ,.
              ;;JuBML::r:.:.,,        SZPX0SXSP5kXGNP15UBr.
                  L,    :@huhao.      :MNZqNXqSqXk2E0PSXPE .
              viLBX.,,v8Bj. i:r7:,     2Zkqq0XXSNN0NOXXSXOU 
            :r2. rMBGBMGi .7Y, 1i::i   vO0PMNNSXXEqP@Secbone.
            .i1r. .jkY,    vE. iY....  20Fq0q5X5F1S2F22uuv1M; 

            小伙子，小姑娘后悔自己没有好好学习了吧 
*/

namespace ws{
    // 每次构造响应报文的时候有大量的调用，见provider.cc
    int __attribute__((hot)) WriteLoop::swrite(const char* format, ...){
        va_list va;
        va_start(va, format);
        int ret = User_Buffer_->SWrite(format, va);
        va_end(va);
        write(ret);
        return ret;
    }

    /**
     * @param: length为要发送数据的长度
    */
    WriteLoop::COMPLETETYPE __attribute__((hot)) WriteLoop::Send(int length){
        if(length < 1) throw std::invalid_argument("'WriteLoop::Send' error parameter.");
        int sent_ = 0;

        for(int ans;
            length - sent_          // 有一次发送失败就退出循环，原因是内核写入队列已满，不应该空转等待；ENOBUFS
            && (ans = static_cast<int>(send(fd_, User_Buffer_->ReadPtr(),length - sent_, 0))) > 0;){
                sent_ += ans;
        }

        throughout += sent_;
        ++interval;

        User_Buffer_->read(sent_);  // 一个http响应报文至少需要两次send，所以缓冲区需要记录长度；
        int Remaining = length - sent_; 
        bool faultError = false;
        // https://man7.org/linux/man-pages/man2/send.2.html
        if (errno != EWOULDBLOCK && errno != EAGAIN){
            if (errno == EPIPE || errno == ECONNRESET){
                // 服务端收到RST报文后recv出现ECONNRESET;向一个已关闭的套接字写数据的时候出现EPIPE；
                // 此时显然我们没有必要再去维护发送缓冲区了，直接返回true就可以了，因为这个套接字已经无效了；
                faultError = true;
            }
        }
        if (!faultError && Remaining > 0){//write failture.
            InsertSend(Remaining);
            return IMCOMPLETE;
        }
        return COMPLETE;
    }

    WriteLoop::COMPLETETYPE __attribute__((hot)) WriteLoop::SendFile(std::shared_ptr<FileReader> ptr){
        ssize_t len = 0;
        ++interval;

        // 对于大文件的发送跑一个优化，没等到数据到最大包长的时候才发送；
        // 当然为此付出两个系统调用的代价是否是值得的也不好说，性能测试上没啥大差别，当然和文件大小太小可能也有关系；
        int on = 1;
        setsockopt(fd_, SOL_TCP, TCP_CORK, &on, sizeof (on)); /* cork */ 
        while(len = ptr->SendFile(fd_) && len > 0){
            throughout += len;
        }
        on = 0; 
        setsockopt(fd_, SOL_TCP, TCP_CORK, &on, sizeof (on)); /* 拔去塞子 */ 

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

    /**
     * @notes: 可能返回IMCOMPLETE和EMPTY，即失败与成功，需要函数调用方判断；
    */
    WriteLoop::COMPLETETYPE WriteLoop::DoAll(){
        while(1){
            auto CompleteType = DoFirst();
            if(interval >= expectedInetrval){
                WriteLoopCallback(throughout);
                throughout = 0;
                interval = 0;
            }
            if(CompleteType == COMPLETE) continue;
            else return CompleteType;
        }
    }

}