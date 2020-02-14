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