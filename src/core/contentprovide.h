 #ifndef CONTENTPROVIDER_H_
 #define CONTENTPROVIDER_H_

#include"../base/nocopy.h"
#include"../net/writeloop.h"
#include"../http/httprequest.h"
#include"provider/provider.h"
#include"provider/reaprovider.h"
#include<memory>

namespace ws{

    class ContentProvider : public Nocopy{
        public:
            ContentProvider(std::shared_ptr<HttpRequest> Hr, std::shared_ptr<WriteLoop> Wl) : 
                Http_Request(Hr), Write_Loop(Wl){}
            void Provide();
        private: 
            std::unique_ptr<Provider> Pro;
            std::shared_ptr<HttpRequest> Http_Request;
            std::shared_ptr<WriteLoop> Write_Loop;
    };
}

#endif