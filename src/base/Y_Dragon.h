#ifndef Y_DRAGON_H_
#define Y_DRAGON_H_

#include"../http/httprequest.h"
#include"../net/address.h"
#include<memory>

namespace Y_Dragon{

    constexpr char* Version(){
        return (char*)"0.0";
    }
    
    int MyPort();

    int EventResult_Number();
}

#endif