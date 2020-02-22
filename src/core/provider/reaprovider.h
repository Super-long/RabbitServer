#ifndef REAPROVIDER_H_
#define REAPROVIDER_H_

#include "provider.h"
#include "../../net/writeloop.h"
#include "../../tool/filereader.h"   
#include "../../http/httprequest.h"
#include "../../tool/parsed_header.h"
#include <memory> 

namespace ws{

    class REAProvider : public Provider{
        public:
            REAProvider(std::shared_ptr<HttpRequest> Hr,
                        std::shared_ptr<WriteLoop> Wl) : 
                        Provider(Hr, Wl){}
            void provide() final;
        private: 
            bool FileProvider(std::shared_ptr<FileReader>& file);
    };

}

#endif