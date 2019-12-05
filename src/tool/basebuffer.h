#ifndef BASEBUFFER_H_
#define BASEBUFFER_H_

#include<cstddef>
#include<memory>

namespace ws{
    class BaseBuffer{
        public:
            virtual size_t Readable() const = 0;
            virtual const char* ReadPtr() const = 0;
            virtual std::unique_ptr<char[]> Read(int bytes) = 0;
            virtual void Read(char* Start, int bytes) = 0;
            virtual size_t Writeable() const = 0;
            virtual char* WritePtr() = 0;
            virtual char Peek(int jump) const = 0;
            virtual size_t Length() const = 0;
    };
}




#endif