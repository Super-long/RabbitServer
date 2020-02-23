#ifndef Y_DRAGON_H_
#define Y_DRAGON_H_

#include "../http/httprequest.h"
#include "../net/address.h"
#include <memory>

namespace Y_Dragon{

    constexpr char* Version(){
        return (char*)"0.0";
    }
    
    constexpr int MyPort();

    constexpr char* MyIP();

    constexpr int FastCgiPort();

    constexpr char* FastCgiIP();

    constexpr int EventResult_Number();

}

#endif