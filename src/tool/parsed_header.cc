#include"parsed_header.h"

#include<iostream>

namespace ws{

    bool ParsedHeader::ParsedHeaderIsEqual(const ParsedHeader& para) const{
        if(length != para.Readable()) return false;
        if(memcmp(Header, para.ReadPtr(), length) == 0) return true;
        return false; 
    }

    bool ParsedHeader::ParsedHeaderIsEqual(const char* ptr) const{
        if(length != strlen(ptr)) return false;
        if(memcmp(Header, ptr, length) == 0) return true;
        return false;
    }

    bool ParsedHeader::ParsedHeaderIsEqual(const std::string& str) const{
        if(length != str.length()) return false;
        if(memcmp(Header, str.c_str(), length) == 0) return true;
        return false;
    }

    std::ostream& operator<<(std::ostream& os, const ParsedHeader& ptr){
        os.write(ptr.ReadPtr(), ptr.Readable());
        return os;
    }

    bool ParsedHeader::operator==(const ParsedHeader& para) const{
        bool Len_is_equal = (length == para.Readable());
        return (Len_is_equal && (strncmp(Header, para.ReadPtr(), length) == 0));
    }
}