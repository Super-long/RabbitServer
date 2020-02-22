#include "http/FastCgi/fastcgi.h"
#include <iostream>

#include <thread>

void test(){
    ws::FastCgi fc;
    fc.start("/home/lizhaolong/suanfa/Web_Server/WebServer/src/http/FastCgi/CGIProgram/index.php","hello world");
    std::string str(fc.ReadContent());
    std::cout << str << std::endl;
}

int main(){
    auto Son = std::thread(test);
    Son.join();
    return 0;
}