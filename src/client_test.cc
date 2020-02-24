#include "client/client.h"
#include <thread>
#include <string>
#include <unistd.h>
#include <iostream>

ws::Client client_;

void test(){
    client_.Run();    
}

const char Content[] = R"(GET server.dot HTTP/1.1
Host: /home/lizhaolong/suanfa/Web_Server/WebServer
User-Agent: curl/7.52.1
Connection: Close

)";

void Test(int fd){
    std::string str(Content);
    std::cout << str << std::endl;
    ::write(fd, str.c_str(), str.length());
    getchar();
}

int main(){
    client_.Connect();
    sleep(1);
    auto son = std::thread(test);
    
    sleep(1);
    int fd = client_.TestInterface();
    std::cout << fd << std::endl;
    client_.Push(std::bind(Test, fd));
    son.join();
    return 0;
}