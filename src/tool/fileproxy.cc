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

#include "fileproxy.h"
#include <memory>

#include <unistd.h>
#include <fcntl.h>

namespace ws{

    void FileProxy::Statget(){
        if(!stat_){
            stat_ = std::make_unique<struct stat>();
            fstat(File_Description, stat_.get());
        }
    }
    
    __off_t FileProxy::FileSize(){
        Statget();
        return stat_->st_size;
    }

    //Refuse directory, character device, block device.
    bool FileProxy::IsTextFile(){
        Statget();
        return (stat_->st_mode & S_IFDIR || stat_->st_mode & S_IFCHR || stat_->st_mode & S_IFBLK);
    }

    FileProxy::~FileProxy(){
        ::close(File_Description);
    }

    FileProxy::FileProxy(const FileProxy& path1, const char* path2) : 
        File_Description(openat(path1.fd(), path2, O_RDONLY)){
        //File_Description(openat(AT_FDCWD, path2, O_RDONLY)){
        //File_Description(open(path2, O_RDONLY)){
        }

}