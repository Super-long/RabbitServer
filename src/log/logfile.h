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

#ifndef LOGFILE_H
#define LOGFILE_H

#include <memory>
#include <mutex>
#include <string>

#include "../base/nocopy.h"
#include "fileappend.h"

namespace ws{

namespace detail{

// 同步日志
class logfile : public Nocopy{
public:
    logfile(const std::string& baseName,
            off_t rollSize,
            bool threadSafe = true,
            int flushInterval = 15,
            int checkEveryN = 1024);
    void append(const char* logline, int len);
    void flush();
    void rollFile();

private:
    void append_unlocked(const char* logline, int len);

    static std::string getlogfileName(const std::string& basename, time_t* now);

    const std::string basename_;
    const off_t rollSize_;
    const int flushInterval_;
    const int checkEveryN_;

    int count_;

    std::unique_ptr<std::mutex> mutex_;
    time_t startOfPeriod_;
    time_t lastRoll_;
    time_t lastFlush_;
    std::unique_ptr<FileAppend> file_;

    constexpr const static int Daypreseconds = 60*60*24;
};

}

}

#endif //LOGFILE_H