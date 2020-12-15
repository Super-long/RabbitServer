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

#include "reaprovider.h"
#include "../parsed_header.h"
#include "../../tool/filereader.h"
#include "../../http/httpstatus.h"
#include "../../FastCgi/fastcgi.h"

#include <string>

namespace ws{

    /**
     * @notes: 根据状态机解析的不同结果，执行不同的行为；
    */
    // TODO 这里搞一个信号槽也许是个更好的选择，但是现在能跑，目前也就不想那么多了；
    void REAProvider::provide(){
        std::shared_ptr<FileReader> file = nullptr; 

        // 目前是同步的，因为仅想实现功能，但是这样写是绝对不可以的，会导致此线程处理用户的RTT都增加；
        // 简单的处理方案是引入libgo去处理这种事件，而不是把这玩意在放到epoll中，当然后者更加简洁；
        if(_Request_->Return_Method() == HRPost){ // fastcgi.
            FastCgi fc;
            auto x = _Request_->Get_Value(static_cast<ParsedHeader>("Host"));
            std::string Host(x.ReadPtr(), x.Readable()+1);
            Host[x.Readable()] = '\0';

            auto y = _Request_->Return_Uri();
            std::string Filename(y.ReadPtr(), y.Readable()+1);
            Filename[x.Readable()] = '\0';
            fc.start(Host + "/" + Filename, "hello world"); //参数随便写的
            
            std::string Content(fc.ReadContent()); //Less efficient.
            
            if(!Good()){
                _Request_->Set_StatusCode(HSCBadRequest);
                ProvideError();
                return;
            }
            _Request_->Set_StatusCode(HSCOK);
            int ret = RegularProvide(Content.size()); //暂时无排错机制
            ret += WriteCRLF();
            ret += _Write_Loop_->write(Content.c_str(), Content.size());
            ret += WriteCRLF(); 
            _Write_Loop_->AddSend(ret); // Send message.

        }else if(FileProvider(file)){   // 这里证明文件解析和打开是没有任何问题的
            int ret = RegularProvide(file->FileSize()); 
            ret += WriteCRLF();         // 请求头和内容之间加上CRLF

            // 向writeloop中加入三个写任务
            _Write_Loop_->AddSend(ret);
            _Write_Loop_->AddSendFile(file);

            ret = WriteCRLF(); 
            _Write_Loop_->AddSend(ret);

        }else{
            ProvideError();
        }
    }

    bool REAProvider::FileProvider(std::shared_ptr<FileReader>& file){
        // TODO 这里其实有一个小bug，当解析到内容而其中内容不够的时候还是失败的，这里无法检测出来；这里有一个交PR的机会，抓住它！
        if(!Good()){    // 没解析到内容说明解析失败；
            _Request_->Set_StatusCode(HSCBadRequest);
            return false;
        }

        // 貌似我当年对host字段有一点点小小的误解；host其实是为了发现一台机器上不同的主机的，可能其他主机跑在虚拟机中；
        auto x = _Request_->Get_Value(static_cast<ParsedHeader>("Host"));
        std::string str(x.ReadPtr(), x.Readable()+1);
        str[x.Readable()] = '\0';

        auto y = _Request_->Return_Uri();
        std::unique_ptr<char[]> ptr2(new char(y.Readable()+1));
        auto release_ptr2 = ptr2.release();
        memcpy(release_ptr2, y.ReadPtr(), y.Readable());
        release_ptr2[y.Readable()] = '\0';

        //std::cout << "str " << str << std::endl;
        //std::cout << "release1 : " << release_ptr2 << std::endl;

        // 参数为 路径+文件名
        // TODO 可以在这里设置一个类似slab的cache层，但是其实实现起来并不容易，因为每个线程一个slab就很浪费资源，但是只有一个的话线程安全的开销又比较大；
        // 这里我们需要特判下默认的情况，即url为'/'
        //std::cout << y.Readable() << " " << release_ptr2[0] << std::endl;
        if(y.Readable() == 1 && release_ptr2[0] == '/'){    // 默认情况打开index.html
            // TODO 这里需要搞成环境变量才可以设置不基于可执行文件的相对路径，麻烦，要么强迫用户以特定的方式执行，要么强迫用户添加环境变量，还是前者省事；
            file = std::make_shared<FileReader>("../src/index.html");
        } else {
            file = std::make_shared<FileReader>   
            ( static_cast<FileProxy>(str.c_str())   // 构造函数中已经open了
            , release_ptr2 );
        }

        // 下面这两条逻辑有点错乱，因为不清楚如何设置错误类型；

        if(!file->Fd_Good() || file->IsTextFile()){ // fd正确且文件类型正确；
            // 可能权限错误也会打开失败；
            _Request_->Set_StatusCode(HSCForbidden);
            return false;
        }

        // TODO 当然还可能是其他的情况，所以这里其实处理的不好，直接看man手册吧
        // https://man7.org/linux/man-pages/man2/open.2.html
        if(!file){ // 智能指针创建失败
            _Request_->Set_StatusCode(HSCInternalServerError);
            return false;
        }

        // 文件打开没有问题就设置状态为OK
        _Request_->Set_StatusCode(HSCOK);
        return true;
    }
}