#include "client/client.h"
#include <thread>
#include <string>
#include <unistd.h>
#include <iostream>

ws::Client client_; 

const char Content[] = R"(********GET server.dot HTTP/1.1
Host: /home/lizhaolong/suanfa/Web_Server/WebServer
User-Agent: curl/7.52.1
Connection: Close

)";

int main(){
    //client_.Connect();
    //sleep(1);
    client_.Start(); //开启事件循环
    client_.Connect(); //Connect与SendToServer的第一次执行要有一个同步关系
    sleep(1);
    client_.SendToServer(Content);
    return 0;
}