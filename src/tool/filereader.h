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

#ifndef _FILEREADER_H_
#define _FILEREADER_H_

#include "fileproxy.h"
#include <string>  

namespace ws{
    class FileReader : public FileProxy{ 
        public:
            explicit FileReader(const char* path) : FileProxy(path){}
            FileReader(const FileProxy& proxy, const char* path) : FileProxy(proxy, path){}
            FileReader(const FileProxy& proxy, const std::string& str) : FileProxy(proxy, str.c_str()){}

            ssize_t SendFile(int Socket_Fd);
            bool Send_End() const{ return length == offest;}

        private:
            __off_t length = FileSize(); 
            __off_t offest = 0;
    };
}

#endif