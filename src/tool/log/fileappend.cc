#include "fileappend.h"
#include "logging.h"
#include <assert.h>

namespace ws{

namespace detail{

    FileAppend::FileAppend(const std::string& filename) 
    : fp_(::fopen(filename.c_str(), "ae")), //e -> O_CLOEXEC
    writtenBytes_(0){
        assert(fp_);
        ::setbuffer(fp_, buffer_, sizeof buffer_); //指定文件缓冲区
    }

    void
    FileAppend::append(const char* logline, size_t len) {
        size_t n = Write(logline, len);
        size_t remain = len - n;
        while (remain > 0){ //写入不完全的情况
            size_t x = Write(logline + n, remain);
            if (x == 0){
                int err = ferror(fp_);
                if (err){//写入出错的时候可以使用ferror检查 非零出错
                    fprintf(stderr, "FileAppend::append() failed %s\n", strerror_tl(err));
                }
                break;
            }
            n += x;
            remain = len - n; // remain -= x
        }
        writtenBytes_ += len;
    }

    size_t
    FileAppend::Write(const char *logline, size_t len) {
        //使用非线程安全的write是因为外面已经加锁 这里保证是线程安全
        return ::fwrite_unlocked(logline, 1, len, fp_);
    }

}

}