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

#ifndef FILEPROXY_H_
#define FILEPROXY_H_

#include "../base/nocopy.h"
#include "../base/havefd.h"
#include <sys/stat.h>
#include <memory>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>

namespace ws{

    class FileProxy: public Nocopy, public Havefd{ 
        public:
            explicit FileProxy(const char* path) : File_Description(::open(path, O_RDONLY)){}
            FileProxy(const FileProxy&, const char*); 
            ~FileProxy() override;

            int fd() const & noexcept final {return File_Description;}
            bool Fd_Good() const noexcept {return File_Description > -1;}
            __off_t FileSize();
            bool IsTextFile(); 

        private:
            int File_Description;
            std::unique_ptr<struct stat> stat_ = nullptr;
            void Statget();
    };

}

#endif