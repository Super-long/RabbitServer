#ifndef WS_H_
#define WS_H_

#include"../net/server.h"
#include"../net/epoll.h"
#include"manger.h"

namespace ws{
    class Web_Server{
        public:
            Web_Server();
            void Running();
        private:
            Manger _Manger_;
            Epoll _Epoll_;
            Server _Server_; 
    };
}

#endif