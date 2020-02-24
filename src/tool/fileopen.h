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

#include <mutex>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../base/nocopy.h"

namespace ws{

class fileopen : public Nocopy{
private:
    std::mutex mutex_;
    int File_Description;
    constexpr const static char* prevent_busyloop = "/dev/null";
public:
    //https://blog.csdn.net/wdzxl198/article/details/6876879
    fileopen() : File_Description(::open(prevent_busyloop, O_RDONLY | O_CREAT | O_EXCL)){}

    void Close(){
        std::lock_guard<std::mutex> guard(mutex_); //It will be slow, but it must not ne wrong.
        ::close(File_Description);
    }

    void Open(){
        std::lock_guard<std::mutex> guard(mutex_);
        ::open(prevent_busyloop,O_RDONLY);
    }
};

class fileopen_helper{
private:
fileopen& File_;
public:
    explicit fileopen_helper(fileopen& File) : File_(File){
        File_.Close();
    }

    ~fileopen_helper(){
        File_.Open();
    }
};

}