/**
 * Copyright lizhaolong(https://github.com/Super-long)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include <assert.h>

#include "logstream.h"

namespace ws{

namespace detail{

//更快的转换
// Efficient Integer to String Conversions, by Matthew Wilson.
const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
const char digitsHex[] = "0123456789ABCDEF";

template<typename T>
size_t convert(char buf[], T value){
    T i = value;
    char* p = buf;
    do{
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0){
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

size_t convertHex(char buf[], uintptr_t value){
    uintptr_t i = value;
    char* p = buf;

    do{
        int lsd = static_cast<int>(i % 16);
        i /= 16;
        *p++ = digitsHex[lsd];
    } while (i != 0);

    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

template<typename T>
void
logstream::formatInteger(T v){
    if (buffer_.avail() >= kMaxNumericSize){
        size_t len = convert(buffer_.current(), v);
        buffer_.add(len);
    }
}

logstream& logstream::operator<<(short v){
    *this << static_cast<int>(v);
    return *this;
}

logstream& logstream::operator<<(unsigned short v){
    *this << static_cast<unsigned int>(v);
    return *this;
}

logstream& logstream::operator<<(int v){
    formatInteger(v);
    return *this;
}

logstream& logstream::operator<<(unsigned int v){
    formatInteger(v);
    return *this;
}

logstream& logstream::operator<<(long v){
    formatInteger(v);
    return *this;
}

logstream& logstream::operator<<(unsigned long v){
    formatInteger(v);
    return *this;
}

logstream& logstream::operator<<(long long v){
    formatInteger(v);
    return *this;
}

logstream& logstream::operator<<(unsigned long long v){
    formatInteger(v);
    return *this;
}

//十六进制的转换
logstream& logstream::operator<<(const void* p){
    uintptr_t v = reinterpret_cast<uintptr_t>(p);
    if (buffer_.avail() >= kMaxNumericSize)
    {
        char* buf = buffer_.current();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = convertHex(buf+2, v);
        buffer_.add(len+2);
    }
    return *this;
}

logstream& logstream::operator<<(double v){
    if (buffer_.avail() >= kMaxNumericSize)
    {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
        buffer_.add(len);
    }
    return *this;
}

template<typename T>
Fmt::Fmt(const char* fmt, T val)
{
  length_ = snprintf(buf_, sizeof buf_, fmt, val);
  assert(static_cast<size_t>(length_) < sizeof buf_);
}

template Fmt::Fmt(const char* fmt, char);

template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);

template Fmt::Fmt(const char* fmt, float);
template Fmt::Fmt(const char* fmt, double);

}

}