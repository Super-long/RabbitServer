#ifndef LOGGING_H_
#define LOGGING_H_

#include <string>
#include <string.h>
#include <functional>
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

#define LOG_DEBUG if (logging::Loglevel() <= logging::DEBUG) \
  logging(__FILE__, __LINE__, logging::DEBUG, __func__).stream()
/* #define LOG_INFO if (logging::Loglevel() <= logging::INFO) \
  logging(__FILE__, __LINE__).stream() */
  #define LOG_INFO logging(__FILE__, __LINE__).stream()
#define LOG_WARN logging(__FILE__, __LINE__, logging::WARN).stream()
#define LOG_ERROR logging(__FILE__, __LINE__, logging::ERROR).stream()
#define LOG_FATAL logging(__FILE__, __LINE__, logging::FATAL).stream()

const char* strerror_tl(int savedErrno);

}

}

#endif //LOGGING_H_