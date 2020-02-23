#include "config.h"

namespace Y_Dragon{
    // 1. Too many requests from the same IP.
    // 2. IP is blacklisted.
    // 3. Insufficient file permissions.
//    bool IsForbidden(std::shared_ptr<ws::HttpRequest>){
//    }

    constexpr int MyPort(){
        return 8888;
    }

    constexpr int FastCgiPort(){
        return 9000;
    }

    constexpr int EventResult_Number(){
        return 8096;
    }

    constexpr char* FastCgiIP(){
        return "127.0.0.1";
    }

    constexpr char* MyIP(){
        return "127.0.0.1";
    }

}