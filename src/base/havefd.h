#ifndef HAVEFD_H_
#define HAVEFD_H_

namespace ws{
    class Havefd{
        public:
            virtual int fd() const = 0; 
            
            bool operator==(const Havefd& para){
                return fd() == para.fd();
            }
    };
}

#endif 