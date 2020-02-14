#ifndef PARSED_HEADER_H_
#define PARSED_HEADER_H_

#include "../base/copyable.h"
#include "basebuffer.h"
#include <memory>
#include <string>
#include <cstring>
#include <fstream> 
#include <functional>
#include <iostream>

namespace ws{

    class ParsedHeader : public Copyable, public BaseBuffer{
        public:
            explicit ParsedHeader(const char* ptr) : length(strlen(ptr)),Header(ptr){}        
            ParsedHeader(const char* ptr,int len) :
                length(static_cast<size_t>(len)),Header(ptr){}
            ParsedHeader() = default;
            size_t Readable() const override {return length;}
            const char* ReadPtr() const override{return Header;}
            void Read(char* Start, int bytes){
                memcpy(Start, Header, static_cast<size_t>(bytes));
            }
            
            virtual std::unique_ptr<char[]> Read(int bytes){
                //This is a design error. 
            }
            size_t Writeable() const{
                return length;
            }
            virtual char* WritePtr(){
                return const_cast<char*>(Header);
            }


            size_t Length() const {return length;}
            char Peek(int jump) const{
                if(jump > length) throw std::out_of_range("'ParsedHeader Peek' Out of max length.");
                return Header[jump];
            }

            bool operator==(const ParsedHeader& ) const;
            bool ParsedHeaderIsEqual(const ParsedHeader& ) const;
            bool ParsedHeaderIsEqual(const char* ) const;
            bool ParsedHeaderIsEqual(const std::string& ) const;

            ~ParsedHeader(){}

        private:
            const char* Header;//获取到的指针本来就是智能指针 
            size_t length;
    };

    std::ostream& operator<<(std::ostream&, const ParsedHeader&);
    
    struct ParseHeaderHash{
        size_t operator()(const ParsedHeader& ptr) const {
            return std::hash<const char*>()(ptr.ReadPtr()) + std::hash<size_t>()(ptr.Readable());
        }
    };
}



#endif