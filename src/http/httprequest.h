#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_

#include <unordered_map>
#include "../base/nocopy.h"
#include "../tool/userbuffer.h"
#include "../tool/parsed_header.h"
#include "httpstatus.h"

namespace ws{

    class HttpRequest : public Nocopy{
        public:
            HttpRequest(){
                Header_Value.reserve(10);
            };
            //
            void Set_VMajor(int ma){ Version_Major = ma;} 
            void Set_VMinor(int mi){ Version_Minor = mi;}
            void Set_CStart(const char* ptr){ Content_Start = ptr;}
            void Set_CLength(size_t len){ Content_Length = len;}

            void Set_Method(const HttpRequestMethod& method){ Method_  = method;}
 
            void Set_Flag(HttpFlag flag){ Flag_ = flag;}
            void Set_StatusCode(const HttpStatusCode& status){ StatusCode_ = status;}
            void Set_Fault(const HttpParserFault& fault){Fault_ = fault;}

            void Set_Uri(const ParsedHeader& ph){Uri_ = ph;}
            void Set_Request_Buffer(std::shared_ptr<UserBuffer> ub){Request_Buffer_ = std::move(ub);}
            //The above is the assignment of the base data.

            //
            int Return_Version_Ma(){return Version_Major;}
            int Return_Version_Mi(){return Version_Minor;}
            const char* Return_Content_Start(){return Content_Start;}
            size_t Return_Content_length(){return Content_Length;}

            HttpStatusCode Return_Statuscode() const {return StatusCode_;}
            HttpRequestMethod Return_Method() const {return Method_;}
            HttpFlag Return_Flag() const {return Flag_;}
            HttpParserFault Return_Fault() const {return Fault_;}

            ParsedHeader& Return_Uri(){return Uri_;}
            std::shared_ptr<UserBuffer> Return_RBuffer(){return Request_Buffer_;}
            //The above is the return of the base data. 

            void Store_Header(const ParsedHeader&, const ParsedHeader&);
            ParsedHeader Get_Value(const ParsedHeader&) const;

            bool Request_good() const{
                //std::cout <<  "Fault " << Fault_  << std::endl; 
                return Fault_ == HPFContent;}

        private:
            int Version_Major;
            int Version_Minor;
            const char* Content_Start;
            size_t Content_Length;

            HttpStatusCode StatusCode_;
            HttpRequestMethod Method_;
            HttpFlag Flag_; 
            HttpParserFault Fault_;
            
            ParsedHeader Uri_; 
            std::shared_ptr<UserBuffer> Request_Buffer_;
            std::unordered_map<ParsedHeader,ParsedHeader,ParseHeaderHash> Header_Value;
    };
}

#endif