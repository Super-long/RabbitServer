#ifndef ADDRESS_H_
#define ADDRESS_H_

#include"../base/copyable.h"
#include<arpa/inet.h>
namespace ws{
    class Address : public Copyable{
        public:
            explicit Address(const struct sockaddr_in& para) : addr_(para){}
            Address(const char* IP, int port);
            explicit Address(int port);

            const sockaddr* Return_Pointer(){return static_cast<const sockaddr*>(static_cast<void*>(&addr_));}
            constexpr size_t Return_length(){return sizeof(struct sockaddr_in);} 
        private:
            struct sockaddr_in addr_;
    };
}


#endif 