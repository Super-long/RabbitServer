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

#ifndef FILEAPPEND_H_
#define FILEAPPEND_H_

#include <string>
#include <memory>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../base/nocopy.h"

namespace ws{

namespace detail{

class FileAppend : public Nocopy
{
public:
    explicit FileAppend(const std::string& filename);

    void append(const char* logline, size_t len);

    ~FileAppend() {::fclose(fp_);}

    void flush() {::fflush(fp_);}

    off_t writtenBytes() const { return writtenBytes_; }

private:

    size_t Write(const char* logline, size_t len);

    FILE* fp_;
    char buffer_[64*1024];
    off_t writtenBytes_;
};

}

}

#endif //FILEAPPEND_H_