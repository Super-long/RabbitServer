#include "httprequest.h"
#include <algorithm>

namespace ws{
    void HttpRequest::Store_Header(const ParsedHeader& header, const ParsedHeader& value){
        Header_Value.emplace(header, value); 
    }

    ParsedHeader HttpRequest::Get_Value(const ParsedHeader& header) const{
/*         auto T = Header_Value.find(header); 
        if(T == Header_Value.end()){   
            throw std::invalid_argument("'HttpRequest::GrtValue' No such user.");
        }
        return T->second; */  

        //Pointers have different hash values.

        auto temp = std::find_if(Header_Value.begin(), Header_Value.end(), [&header](auto para){
            return para.first.ParsedHeaderIsEqual(header);
        });
        if(temp == Header_Value.end()) throw std::invalid_argument("'HttpRequest::GrtValue' No such user.");
        return temp->second;
    }
}