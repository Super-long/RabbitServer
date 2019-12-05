#include"Y_Dragon.h"

namespace Y_Dragon{
    // 1. Too many requests from the same IP.
    // 2. IP is blacklisted.
    // 3. Insufficient file permissions.
//    bool IsForbidden(std::shared_ptr<ws::HttpRequest>){
//    }

    int MyPort(){
        return 8888;
    }

    int EventResult_Number(){
        return 8096;
    }
}