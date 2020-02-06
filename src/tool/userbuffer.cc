#include"userbuffer.h"
#include <cstring>

namespace ws{
 
    void UserBuffer::Read(char* Start, int bytes) {
        if(Read_Spot + bytes > Write_Spot){
            throw std::out_of_range("'UserBuffer::Read' Out ou range when reading.");
        }
        memcpy(Start,Buffer_.get() + Read_Spot, static_cast<size_t>(bytes));
        Read_Spot += static_cast<size_t>(bytes);
    }

/*     void UserBuffer::Read(std::string& str, int bytes){
        if(Read_Spot + bytes > Write_Spot){
            throw std::out_of_range("'UserBuffer::Read' Out ou range when reading.");
        }
        memcpy(const_cast<char*>(str.c_str()),Buffer_.get() + Read_Spot, static_cast<size_t>(bytes));
        Read_Spot += static_cast<size_t>(bytes);  
    } */

    std::unique_ptr<char[]> UserBuffer::Read(int bytes){
        if(Read_Spot + bytes > Write_Spot){
            throw std::out_of_range("'UserBuffer::Read' Out ou range when reading.");
        }
        std::unique_ptr<char[]> ptr(new char(Readable()));
        memcpy(ptr.get(), Buffer_.get() + Read_Spot, static_cast<size_t>(bytes));
        Read_Spot += static_cast<size_t>(bytes);
        return ptr;
    }

    char UserBuffer::Peek(int jump) const{
        if(Write_Spot < Read_Spot + jump && Read_Spot + jump < 0 ){
            throw std::out_of_range("'UserBuffer::Peek' The parameter is not in the vaild range.");
        }
        return Buffer_[Read_Spot + jump];
    }

    int UserBuffer::Write(int bytes){
        if(bytes > Writeable()){
            throw std::out_of_range("'UserBuffer::Write' The parameter is not in the valid range");
        }
        Write_Spot += bytes;
        Move_Buffer();
    }

    int UserBuffer::Write(char* Buf, int bytes){
        if(bytes > Writeable()){
            throw std::out_of_range("'UserBuffer::Write' The parameter is not in the valid range");
        }
        memcpy(Buffer_.get() + Write_Spot, Buf, static_cast<size_t>(bytes));
        Write_Spot += static_cast<size_t>(bytes);
        return bytes;
    }

    int UserBuffer::Write(const char* Buf, int bytes){
        if(bytes > Writeable()){
            throw std::out_of_range("'UserBuffer::Write' The parameter is not in the valid range");
        }
        memcpy(Buffer_.get() + Write_Spot, Buf, static_cast<size_t>(bytes));
        Write_Spot += static_cast<size_t>(bytes);
        return bytes;
    }

    int UserBuffer::Write(const std::string& str){
        size_t len = str.length();
        if(len > Writeable()){
            throw std::out_of_range("'UserBuffer::Write' The parameter is not in the valid range");
        }
        memcpy(Buffer_.get() + Write_Spot, str.c_str(), len);
        Write_Spot += len;
        return static_cast<int>(len);
    }

    int UserBuffer::SWrite(const char* format, va_list para){
        return vsnprintf(Buffer_.get() + Write_Spot, static_cast<size_t>(Writeable()), format, para);
    }

    void UserBuffer::Move_Buffer(){
        size_t inter = Write_Spot - Read_Spot;
        memcpy(Buffer_.get(), Buffer_.get() + Read_Spot, inter);
        Read_Spot = 0;
        Write_Spot = inter;
    }
}