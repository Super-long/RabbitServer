#ifndef LOGSTREAM_H_
#define LOGSTREAM_H_

#include "Fixedbuffer.h"

namespace ws{

namespace detail{

    class logstream{
        using self = logstream;
    public:
        using Buffer =  FixedBuffer<SmallBuffer>;

        self& operator<<(bool flag){
            buffer_.append(flag ? "1" : "0", 1);
        }

        self& operator<<(short);
        self& operator<<(unsigned short);
        self& operator<<(int);
        self& operator<<(unsigned int);
        self& operator<<(long);
        self& operator<<(unsigned long);
        self& operator<<(long long);
        self& operator<<(unsigned long long);

        self& operator<<(const void*);

        self& operator<<(float v){
            *this << static_cast<double>(v);
            return *this;
        }
        self& operator<<(double);
        // self& operator<<(long double);

        self& operator<<(char v){
            buffer_.append(&v, 1);
            return *this;
        }

        // self& operator<<(signed char);
        // self& operator<<(unsigned char);

        self& operator<<(const char* str){
            if(str){
                buffer_.append(str, strlen(str));
            }else{
                buffer_.append("(null)", 6);
            }
            return *this;
        }

        self& operator<<(const unsigned char* str){
            return operator<<(reinterpret_cast<const char*>(str));
        }

        self& operator<<(const string& v){
            buffer_.append(v.c_str(), v.size());
            return *this;
        }

        self& operator<<(const Buffer& v){
            *this << v.toStringPiece();
            return *this;
        }

        void append(const char* data, int len) { buffer_.append(data, len); }
        const Buffer& buffer() const { return buffer_; }
        void resetBuffer() { buffer_.reset(); }

    private:
        Buffer buffer_;
        constexpr const static int kMaxNumericSize = 32;

        template<typename T>
        void formatInteger(T);
    };

}

}

#endif //LOGSTREAM_H_