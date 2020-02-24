#include <unistd.h>

#include "channel.h"

namespace ws{

/* void 
ClientChannel::push(const std::function<void()>& fun){
    ::write(Eventfd.fd(), &padding, sizeof padding);
    Events.push(std::move(fun));
}

void
ClientChannel::RunAndPop(){
    uint64_t Temp = 0;
    ::read(Eventfd.fd(), &Temp, sizeof(Temp));
    while(Temp--){
        std::function<void()> fun = std::move(Events.front());
        Events.pop();
        fun();
    }
}
 */
}