#ifndef EPOLLEVENT_H_
#define EPOLLEVENT_H_

#include "../base/havefd.h"
#include "../base/copyable.h"
#include <sys/epoll.h>
#include <initializer_list>
 
namespace ws{ 
    enum EpollEventType{
        EETCOULDREAD  = ::EPOLLIN,
        EETCOULDWRITE = ::EPOLLOUT,
        EETEDGETRIGGER= ::EPOLLET,
        EETRDHUP      = ::EPOLLRDHUP,
        EETONLYONE    = ::EPOLLONESHOT,
        EETERRNO      = ::EPOLLERR,
        EETPRI        = ::EPOLLPRI,
        EETHUP        = ::EPOLLHUP
    };

    constexpr EpollEventType EpollTypeBase() {return static_cast<EpollEventType>(EETEDGETRIGGER | EETONLYONE | EETRDHUP); }
    constexpr EpollEventType EpollCanRead() {return static_cast<EpollEventType>(EpollTypeBase() | EETCOULDREAD); }
    constexpr EpollEventType EpollCanWite() {return static_cast<EpollEventType>(EpollTypeBase() | EETCOULDWRITE); }
    constexpr EpollEventType EpollRW() {return static_cast<EpollEventType>(EpollTypeBase() | EETCOULDWRITE | EETCOULDREAD); }
    
    class EpollEvent final : public Copyable{
        public:
            EpollEvent() : event_(){}                   //这里的原因是因为epoll_event这个结构体中还包含者一个共用体
            explicit EpollEvent(int fd) : event_(epoll_event{EpollRW(),{.fd = fd}}){}
            EpollEvent(int fd,EpollEventType EET) : event_(epoll_event{EET,{.fd = fd}}){}
            EpollEvent(const Havefd& Hf,EpollEventType EET) : 
                event_(epoll_event{EET,{.fd = Hf.fd()}}){} //这样可以被Havefd的派生类构造 其中包含fd 可行

            bool check(EpollEventType EET){return event_.events & EET;}
            bool check(std::initializer_list<EpollEventType> EET){
                for(auto T : EET){
                    if(!(event_.events & T)) return false;
                }
                return true;
            }
            epoll_event* Return_Pointer()noexcept {return &event_;}
            int Return_fd()const noexcept {return event_.data.fd;}
            uint32_t Return_EET()const noexcept {return event_.events;}
        private:
            epoll_event event_;
    };
}

#endif 