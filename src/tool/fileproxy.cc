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
    
    // 保证换机器不会出现数字越界，所以不转int之类的；
    __off_t FileProxy::FileSize(){
        Statget();
        return stat_->st_size;
    }

    // fstat函数，https://blog.csdn.net/u011003120/article/details/78218989?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522160776531419724838513078%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=160776531419724838513078&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduend~default-4-78218989.first_rank_v2_pc_rank_v29&utm_term=fstat&spm=1018.2118.3001.4449
    // Refuse directory, character device, block device，FIFO device.
    bool FileProxy::IsTextFile(){
        Statget();
        return (stat_->st_mode & S_IFDIR || stat_->st_mode & S_IFCHR || stat_->st_mode & S_IFBLK || stat_->st_mode & S_IFIFO);
    }

    FileProxy::~FileProxy(){
        ::close(File_Description);
    }
    
    // [open()与openat()区别](https://blog.csdn.net/qq_44842973/article/details/103137721?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522160776499419195271691681%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=160776499419195271691681&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_v2~rank_v29-9-103137721.first_rank_v2_pc_rank_v29&utm_term=openat&spm=1018.2118.3001.4449)
    FileProxy::FileProxy(const FileProxy& path1, const char* path2)
        : File_Description(openat(path1.fd(), path2, O_RDONLY)){
        //File_Description(openat(AT_FDCWD, path2, O_RDONLY)){
        //File_Description(open(path2, O_RDONLY)){
        }

}