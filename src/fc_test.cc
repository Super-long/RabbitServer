#include "http/FastCgi/fastcgi.h"
#include <iostream>
int main(){
    ws::FastCgi fc;
    fc.start("/home/lizhaolong/suanfa/Web_Server/WebServer/src/http/FastCgi/CGIProgram/index.php","hello world");
    std::string str(fc.ReadContent());
    std::cout << str << std::endl;
    return 0;
}