#ifndef FASTCGI_H_
#define FASTCGI_H_

#include "fastcgiHeader.h"

namespace ws{

class FastCgi{
private:

public:
    //生成头部
    FCGI_Header makeHeader(int type,int request,
                            int contentLength,int paddingLength);
};

}

#endif //FASTCGI_H_