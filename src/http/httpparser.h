#ifndef HTTPPARSER_H_
#define HTTPPARSER_H_

#include "../base/nocopy.h"
#include "../tool/userbuffer.h"
#include "httprequest.h"
#include "httpstatus.h"
#include <memory>

namespace ws{

    class HttpParser : public Nocopy{ 
        public: 
            explicit HttpParser(std::shared_ptr<UserBuffer> ptr, std::shared_ptr<HttpRequest> request):
                User_Buffer_(std::move(ptr)),Parser_Result(std::make_unique<HttpParser_Content>()),Request_Result(request){}

            void Again_Parser(); 
            HttpParserFault Starting_Parser(); 
            bool Finished() const{ return Parser_Result->Fault != HPFOK;}
            
            bool SetRequesting();

        private: 
            std::shared_ptr<UserBuffer> User_Buffer_;
            std::unique_ptr<HttpParser_Content> Parser_Result;
            std::shared_ptr<HttpRequest> Request_Result; 
 
            bool Parsering();
            bool Parser_able(){ return User_Buffer_->Readable() >= 16;}
    };

}

#endif