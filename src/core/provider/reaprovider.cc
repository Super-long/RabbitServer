#include"reaprovider.h"
#include"../../tool/parsed_header.h"
#include"../../tool/filereader.h"
#include"../../http/httpstatus.h"

#include <string>

namespace ws{

    void REAProvider::provide(){ 
        std::shared_ptr<FileReader> file = nullptr;
        bool cond = FileProvider(file);
        if(cond){
            int ret = RegularProvide(file->FileSize()); 
            ret += WriteCRLF();
            _Write_Loop_->AddSend(ret);
            _Write_Loop_->AddSendFile(file);
            ret = WriteCRLF();
            _Write_Loop_->AddSend(ret);
        }else{
            ProvideError();
        }
    }

    bool REAProvider::FileProvider(std::shared_ptr<FileReader>& file){

        if(!Good()){
            _Request_->Set_StatusCode(HSCBadRequest);
            return false;
        }

        auto x = _Request_->Get_Value(static_cast<ParsedHeader>("Host"));
        std::string str(x.ReadPtr(), x.Readable()+1);
        str[x.Readable()] = '\0';

        auto y = _Request_->Return_Uri();
        std::unique_ptr<char[]> ptr2(new char(y.Readable()+1));
        auto release_ptr2 = ptr2.release();
        memcpy(release_ptr2, y.ReadPtr(), y.Readable());
        release_ptr2[y.Readable()] = '\0';

/*         file = std::make_shared<FileReader>   
        (static_cast<FileProxy>(_Request_->Get_Value(static_cast<ParsedHeader>("Host")).ReadPtr())
        , _Request_->Return_Uri().ReadPtr());  */

        file = std::make_shared<FileReader>   
        (static_cast<FileProxy>(str.c_str())
        , release_ptr2);

        if(!file->Fd_Good() || file->IsTextFile()){ 
            _Request_->Set_StatusCode(HSCForbidden); 
            return false;
        }
        if(!file){
            _Request_->Set_StatusCode(HSCNotFound);
            return false;
        }
        _Request_->Set_StatusCode(HSCOK);
        return true;
    }
}