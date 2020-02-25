/**
 * Copyright lizhaolong(https://github.com/Super-long)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

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
        Http_Parser_ = std::make_unique<HttpParser>(User_Buffer, Http_Request_ ,Socket_Ptr->ReturnExtraBuffer()); 
        Write_Loop_ = std::make_shared<WriteLoop>(fd(), 4048); //outputbuffer
        Content_Provider_ = std::make_unique<ContentProvider>(Http_Request_, Write_Loop_);
    }

    bool Member::CloseAble() const{
        if(Http_Request_->Return_Flag() != Keep_Alive && Http_Parser_->Finished()) return true;
        return false; 
    }
}