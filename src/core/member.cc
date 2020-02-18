#include "member.h"
#include "../tool/userbuffer.h"
#include <memory>

namespace ws{
    void Member::DoRead(){
        Socket_Ptr->Read(User_Buffer); 

        if(Http_Parser_->Finished()){
            Http_Parser_->Again_Parser(); 
        }

        Http_Parser_->Starting_Parser(); 
        if(Http_Parser_->Finished()){
            Content_Provider_->Provide();
        }
        if( Write_Loop_->DoAll() != WriteLoop::IMCOMPLETE){
            WriteComplete = true;
        }
    } 

    void Member::DoWrite(){
        if(Write_Loop_->DoAll() != WriteLoop::IMCOMPLETE){
            WriteComplete = true;
        }else{
            WriteComplete = false;
        }
    } 
 
    void Member::Init(){ //TODO buffer需要修改
        User_Buffer = std::make_shared<UserBuffer>(4048);//inputbuffer
        Http_Request_ = std::make_shared<HttpRequest>();
        Http_Parser_ = std::make_unique<HttpParser>(User_Buffer, Http_Request_); 
        Write_Loop_ = std::make_shared<WriteLoop>(fd(), 4048); //outputbuffer
        Content_Provider_ = std::make_unique<ContentProvider>(Http_Request_, Write_Loop_);
    }

    bool Member::CloseAble() const{
        if(Http_Request_->Return_Flag() != Keep_Alive && Http_Parser_->Finished()) return true;
        return false; 
    }
}