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

#ifndef ASYNCLOGGING_H_
#define ASYNCLOGGING_H_

#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <string>

#include <condition_variable>
#include "../../base/nocopy.h"
#include "Fixedbuffer.h"
  
namespace ws{

namespace detail{
class AsyncLogging : public Nocopy{
public:
    AsyncLogging(const std::string& basename,
                off_t rollsize, //long int //文件缓冲区中需要刷新时的字节数
                size_t flushInterval = 3);
    
    void start();
    void stop();
    void append(const char* line, int len);

    ~AsyncLogging(){
        if(running) {stop();}
    }

private:
    void AsyncFunc();

    using Buffer = FixedBuffer<LargerBuffer>;
    using Buffervector = std::vector<std::unique_ptr<Buffer>>;
    using Bufferptr = typename Buffervector::value_type;

    const std::string basename; 
    const off_t rollsize_;
    const size_t flushInterval_;

    std::thread thread_;
    std::atomic_bool running;
    std::mutex mutex_;
    std::condition_variable cv;
    Bufferptr currentBuffer_; //double buffering.
    Bufferptr nextBuffer_;
    Buffervector buffers_;
};

}

}

#endif //ASYNCLOGGING_H_