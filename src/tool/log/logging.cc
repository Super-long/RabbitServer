#include "logging.h"

#include <errno.h>
#include <string.h>
#include <cstdio>
#include <sstream>
#include <assert.h>


namespace ws{

namespace detail{

thread_local char t_errnoBuf[512];
thread_local char t_time[64];
thread_local time_t t_lastSecond;

const char* strerror_tl(int savedErrno)
{   //thread safe.
    return strerror_r(savedErrno, t_errnoBuf, sizeof(t_errnoBuf));
}

constexpr Logger::LogLevel initLogLevel()
{
if (::getenv("LOG_DEBUG"))
        return Logger::DEBUG;
    else
        return Logger::INFO;
}

constexpr Logger::LogLevel g_logLevel = initLogLevel();
logging::OutputFun g_output(defaultOutput);
logging::FlushFun g_flush(defaultFlush);
//TODO

constexpr const char* LogLevelName[Logger::NUM_LOG_LEVELS] = {
        "DEBUG ",
        "INFO  ",
        "WARN  ",
        "ERROR ",
        "FATAL ",
};

//TODO helper class for known string length at compile time 没搞懂为什么
struct helper{ //TODO 可以到时候改成constexper
     helper(const char* str, unsigned len)
            :str_(str),
             len_(len)
    {
        assert(strlen(str) == len_);
    }

    const char* str_;
    const unsigned len_;
};

inline LogStream& operator<<(logstream& s, const helper& v)
{
    s.append(v.str_, v.len_);
    return s;
}

void defaultOutput(const char* msg, int len){
    size_t n = fwrite(msg, 1, len, stdout);
    //FIXME check n
    (void)n;
}

void defaultFlush(){
    fflush(stdout);
}

void logging::setLoglevel(Loglevel level) {
    g_logLevel = level;
}

void logging::setFlush(FlushFun fun) {
    g_flush = fun;
}

void logging::setOutput(OutputFun fun) {
    g_flush = fun;
}

logging::logging(Filewrapper file, int line)
    : wrapper_(INFO, 0 ,file, line){}

logging::logging(Filewrapper file, int line, logging::Loglevel level)
    : wrapper_(level, 0, file, line){}

logging::logging(Filewrapper file, int line, Loglevel level, const char* str)
    : wrapper_(level, 0, file, line){
    wrapper_.stream_ << str << " ";
}

logging::logging(Filewrapper file, int line, bool toAbort)
    : wrapper_(toAbort?FATAL:ERROR, errno, file, line){}

logging::~logging(){
    wrapper_.finish();
    const logstream::Buffer& buf(stream().buffer());
    g_output(buf.data(), buf.Length());
    if(wrapper_.level_ == FATAL){
        g_flush();
        abort();
    }
}

void logging::Funwrapper::finish() {
    stream_ << " - " << basename_ << ':' << line_ << '\n'
}

void logging::Funwrapper::formatTime() {

}

}

}