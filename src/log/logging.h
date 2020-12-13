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

#ifndef LOGGING_H_
#define LOGGING_H_

#include <mutex>
#include <string>
#include <functional>

#include <string.h>
#include <errno.h>

#include "timestamp.h"
#include "logstream.h"
#include "timezone.h"

namespace ws{ 

namespace detail{

class logging{
public:
    enum Loglevel{
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    template<typename logging::Loglevel LEVEL>
    friend class loggingFactory;

    struct Filewrapper{
        template<int Num>
        Filewrapper(const char (&arr)[Num]) //匹配char数组
            :data_(arr), size_(Num - 1){
                const char* point = strrchr(data_, '/');
                if(point){
                    data_ = point + 1;
                    size_ -= std::distance(arr, data_);
                }
            }

        Filewrapper(const std::string& filename)
            :data_(filename.c_str()){
                const char* point = strrchr(data_, '/');
                if(point){
                    data_ = point + 1;
                }
                size_ = static_cast<int>(strlen(data_));
            }

        const char* data_;
        int size_;
    };

    logging(Filewrapper file, int line);
    logging(Filewrapper file, int line, logging::Loglevel level);
    logging(Filewrapper file, int line, Loglevel level, const char* str);
    logging(Filewrapper file, int line, bool toAbort);
    ~logging();  

    logstream& stream() { return wrapper_.stream_; }

    static Loglevel logLevel();
    inline static void setLoglevel(Loglevel level);
    using OutputFun = std::function<void(const char*, int)>;
    using FlushFun = std::function<void(void)>;
    static void setOutput(OutputFun fun);
    static void setFlush(FlushFun fun);
    static void setTimeZone(const TimeZone& tz);

    //TODO

private:
    struct Funwrapper{
        using Loglevel = logging::Loglevel;
        Funwrapper(Loglevel level, int old_errno, const Filewrapper& file, int line);
        void formatTime();
        void finish();

        Timestamp time_;
        logstream stream_;
        Loglevel level_;
        int line_;
        Filewrapper basename_;
    };

    Funwrapper wrapper_;
};

/*--------------------------------*/

extern logging::Loglevel g_Loglevel; //全局唯一的,即默认日志级别

inline logging::Loglevel logLevel(){
    return g_Loglevel;
}

const char* strerror_tl(int savedErrno);

template<typename logging::Loglevel LEVEL>
class loggingFactory : public Nocopy{
private:
    std::shared_ptr<logging> LogData;
    std::once_flag resourse_flag; 

    void initResourse(logging::Filewrapper file, int line, typename ws::detail::logging::Loglevel level);//{
 
public: 
    logging& getStream(logging::Filewrapper file, int line, int olderrno, typename ws::detail::logging::Loglevel level = logging::INFO);
    ~loggingFactory();
};

logging& log_DEBUG(logging::Filewrapper file, int line, int olderrno);
logging& log_INFO(logging::Filewrapper file, int line, int olderrno);
logging& log_WARN(logging::Filewrapper file, int line, int olderrno);
logging& log_ERROR(logging::Filewrapper file, int line, int olderrno);
logging& log_FATAL(logging::Filewrapper file, int line, int olderrno);

} 

}

#endif //LOGGING_H_