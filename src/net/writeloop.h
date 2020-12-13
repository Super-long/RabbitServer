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

#ifndef WRITELOOP_H_
#define WRITELOOP_H_

#include "../base/nocopy.h"
#include "../tool/userbuffer.h"
#include "../base/havefd.h"
#include "../tool/filereader.h"

#include <deque>
#include <memory>
#include <string>
#include <functional>

namespace ws{

    // WriteLoop本质上是把数据全部放在缓冲区中；
    class WriteLoop : public Nocopy, public Havefd{
        public:
            enum COMPLETETYPE {IMCOMPLETE, COMPLETE, EMPTY};
            using Task = std::function<WriteLoop::COMPLETETYPE()>;

            explicit WriteLoop(int fd, int length = 4096) : fd_(fd),User_Buffer_(std::make_unique<UserBuffer>(length)){} 
            int fd() const & override{return fd_;}

            int write(int bytes) {return User_Buffer_->Write(bytes);}   // 仅增加长度

            int write(char* buf, int bytes) {return User_Buffer_->Write(buf, bytes);}   // 这两个其实一样，一般的指针可以向顶层const转换
            int write(const char* buf, int bytes) {return User_Buffer_->Write(buf, bytes);}
            int write(const std::string& str) {return User_Buffer_->Write(str);}
            int swrite(const char* format, ...);

            int writeable() const{return User_Buffer_->Writeable();} 
            void Move_Buffer() {User_Buffer_->Move_Buffer();}
            size_t WSpot() const noexcept {return User_Buffer_->WSpot();}
            void Rewrite(int spot) noexcept {return User_Buffer_->ReWirte(spot);}
             
            void AddSend(int length){Que.emplace_back([this, length]{return Send(length);});}
            void AddSend(){Que.emplace_back([this]{return Send(User_Buffer_->Readable());});}

            // 用lamda代替bind，事实上在cpp14 bind已经没有什么用了；
            void AddSendFile(std::shared_ptr<FileReader> ptr){Que.emplace_back([this, ptr]{return SendFile(ptr);});}

            COMPLETETYPE DoFirst();
            COMPLETETYPE DoAll();
 
        private:
            std::unique_ptr<UserBuffer> User_Buffer_; 
            std::deque<Task> Que; //支持长连接
            int fd_;
 
            COMPLETETYPE Send(int length);
            COMPLETETYPE SendFile(std::shared_ptr<FileReader>);
            void InsertSend(int len){Que.emplace_front([this,len] {return Send(len);});}
            void InsertSendFile(const std::shared_ptr<FileReader>& ptr) {Que.emplace_back([this, ptr]{return SendFile(ptr);});}
    };
}

#endif 