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

#include <sys/time.h>
#include <cstdio>
#include <inttypes.h>

#include "timestamp.h"

namespace ws{

namespace detail{
Timestamp
Timestamp::now() {
    struct timeval tv;//第一个成员是秒 第二个是微秒
    gettimeofday(&tv, NULL); //并不是系统调用
    uint64_t seconds = tv.tv_sec;
    return Timestamp(seconds * KmicroSecond + tv.tv_usec);
}

std::string
Timestamp::toString() const {
    char buf[32] = {0};
    int64_t seconds = microseconds / KmicroSecond;
    int64_t microseconds = microseconds % KmicroSecond; //跨平台的效果
    std::snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
    return buf;
}

std::string
Timestamp::toFormattedString(bool showMicroseconds) const {
    char buf[64] = {0};
    time_t seconds = static_cast<time_t>(microseconds / KmicroSecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);

    if (showMicroseconds)
    {
        int microseconds = static_cast<int>(microseconds % KmicroSecond);
        std::snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d", //定长 可优化
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                 microseconds);
    }
    else
    {
        std::snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }
    return buf;
}

}

}