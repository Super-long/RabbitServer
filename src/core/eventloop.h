#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_

#include <thread>
#include <assert.h>

#include "../base/nocopy.h"

namespace ws{

class EventLoop : public Nocopy {
private:
bool looping;
const std::thread::id threadID;

public:
    EventLoop();
    ~EventLoop();
    void loop();
};

}

#endif //EVENTLOOP_H_