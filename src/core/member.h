#ifndef MEMBER_H_
#define MEMBER_H_

#include <memory>
#include <algorithm>
#include "../base/havefd.h"
#include "../base/nocopy.h"
#include "../net/socket.h"
#include "../tool/userbuffer.h"
#include "../http/httprequest.h"
#include "../http/httpparser.h"
#include "contentprovide.h"
#include "../net/writeloop.h"
 
namespace ws{
    class Member : public Nocopy,public Havefd{
        public:
            Member(int fd, long time = -1l) : Socket_Ptr(std::make_unique<Socket>(fd)),Time_Spot(time){Init();}
            Member(std::unique_ptr<Socket>&& ptr, long time = -1l) : Time_Spot(time){
                Init();
                std::swap(Socket_Ptr,ptr);
            }
            Member() = delete;
            long TimeSpot() const { return Time_Spot; }
            void Touch(long Time_) { Time_Spot = Time_; }

            void DoRead(); 
            void DoWrite(); 
            bool CloseAble() const; 

            int fd() const final{return Socket_Ptr->fd();} 
            void Init(); 

            ~Member() {Socket_Ptr->Close();}
        private:
            std::unique_ptr<HttpParser> Http_Parser_;
            std::shared_ptr<HttpRequest> Http_Request_;
            std::unique_ptr<Socket> Socket_Ptr;
            std::shared_ptr<WriteLoop> Write_Loop_;
            std::unique_ptr<ContentProvider> Content_Provider_;
            
            std::shared_ptr<UserBuffer> User_Buffer;
            long Time_Spot;
    };
}

#endif 