#include"reaprovider.h"
#include"../../tool/parsed_header.h"
#include"../../tool/filereader.h"
#include"../../http/httpstatus.h"

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
            std::cout << _Request_->Return_Flag() << std::endl;
            _Request_->Set_StatusCode(HSCBadRequest);
            return false;
        }

        auto x = _Request_->Get_Value(static_cast<ParsedHeader>("Host"));
        std::unique_ptr<char[]> ptr1(new char(x.Readable()));
        auto release_ptr1 = ptr1.release();
        memcpy(release_ptr1, x.ReadPtr() , x.Readable());

        auto y = _Request_->Return_Uri();
        std::unique_ptr<char[]> ptr2(new char(y.Readable()));
        auto release_ptr2 = ptr2.release();
        memcpy(release_ptr2, y.ReadPtr(), y.Readable());

/*         file = std::make_shared<FileReader>   
        (static_cast<FileProxy>(_Request_->Get_Value(static_cast<ParsedHeader>("Host")).ReadPtr())
        , _Request_->Return_Uri().ReadPtr());  */

        file = std::make_shared<FileReader>   
        (static_cast<FileProxy>(release_ptr1)
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