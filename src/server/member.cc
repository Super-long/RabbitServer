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
        // step1: 从套接字中把数据读到Member的User_Buffer和Socket的Extrabuf中；
        Socket_Ptr->Read(User_Buffer);

        // TODO 有一个bug，如果包从数据部分被分开，这样就检测不到了，因为是依靠状态来判断解析是否成功的；
        // step2: 做一些解析http请求的前置工作(主要是上一次可能解析过，但是数据包不够大，导致解析失败)；
        if(Http_Parser_->Finished()){       // 检测报文是否大于最小的有效HTTP长度
            Http_Parser_->Again_Parser();   // 做一些解析前的预处理工作，
        }

        // step3: 开始http解析,并把响应头和文件内容全部加入到输出缓冲区中；
        Http_Parser_->Starting_Parser(); 
        if(Http_Parser_->Finished()){       // 不等于HFSOK证明至少解析了一项，也就是者少解析已经开始，成不成功就不一定了；
            Content_Provider_->Provide();   // Http_Request_和Write_Loop_都在Content_Provider_中，这一步把响应头和请求内容插入到输出缓冲区
        }

        // std::cout << "writeloop size : " << Write_Loop_->TaskQueSize() << std::endl;

        // step4: 将输出缓冲区内容发出去
        if(Write_Loop_->DoAll() != WriteLoop::IMCOMPLETE){ 
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
        // 4048限制了接收包的大小
        User_Buffer = std::make_shared<UserBuffer>(4048);       // inputbuffer
        Http_Request_ = std::make_shared<HttpRequest>();
        Http_Parser_ = std::make_unique<HttpParser>(User_Buffer, Http_Request_ ,Socket_Ptr->ReturnExtraBuffer()); 
        Write_Loop_ = std::make_shared<WriteLoop>(fd(), 4048);  // outputbuffer
        Content_Provider_ = std::make_unique<ContentProvider>(Http_Request_, Write_Loop_);
    }

    bool Member::CloseAble() const & {
        if(Http_Parser_->Finished() 
        && ((Http_Request_->Return_Version_Ma() == 1 && Http_Request_->Return_Version_Mi() == 0) || Http_Request_->Return_Flag() != Keep_Alive)) return true;    // http1.0不支持短连接
        //if(Http_Request_->Return_Flag() != Keep_Alive && Http_Parser_->Finished()) return true;
        return false; 
    }
}