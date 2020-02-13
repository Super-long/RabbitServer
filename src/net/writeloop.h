#ifndef WRITELOOP_H_
#define WRITELOOP_H_

#include"../base/nocopy.h"
#include"../tool/userbuffer.h"
#include"../base/havefd.h"
#include"../tool/filereader.h"
#include<deque>
#include<memory>
#include<string>
#include<functional>

namespace ws{

    class WriteLoop : public Nocopy, public Havefd{
            using Task = std::function<bool()>;
        public:
            WriteLoop(int fd, int length) : fd_(fd),User_Buffer_(std::make_unique<UserBuffer>(length)){} 
            int fd() const override{return fd_;}

            int write(int bytes) {User_Buffer_->Write(bytes);}
            int write(char* buf, int bytes) {User_Buffer_->Write(buf, bytes);}
            int write(const char* buf, int bytes) {User_Buffer_->Write(buf, bytes);}
            int write(const std::string& str) {User_Buffer_->Write(str);}
            int swrite(const char* format, ...);

            int writeable() const{return User_Buffer_->Writeable();}
            void Move_Buffer() {User_Buffer_->Move_Buffer();}
            size_t WSpot() const noexcept {return User_Buffer_->WSpot();}
            void Rewrite(int spot) noexcept {return User_Buffer_->ReWirte(spot);}
 
            void AddTask(int len){Que.emplace_back([this, len]{return Send(len);});}
            void AddTask() {AddTask(User_Buffer_->Readable());}
             
            void AddSend(int length){Que.emplace_back([this, length]{return Send(length);});}
            void AddSend(){Que.emplace_back([this]{return Send(User_Buffer_->Readable());});}
            void AddSendFile(std::shared_ptr<FileReader> ptr){Que.emplace_back([this, ptr]{return SendFile(ptr);});}

            bool DoFirst();
            bool DoAll(){while(DoFirst());} //可能阻塞

        private:
            std::unique_ptr<UserBuffer> User_Buffer_; 
            std::deque<Task> Que; //支持长连接
            int fd_;
 
            bool Send(int length);
            bool SendFile(std::shared_ptr<FileReader>);
            void InsertSend(int len){Que.emplace_front([this,len] {return Send(len);});}
            void InsertSendFile(const std::shared_ptr<FileReader>& ptr) {Que.emplace_back([this, ptr]{return SendFile(ptr);});}
    };
}

#endif 