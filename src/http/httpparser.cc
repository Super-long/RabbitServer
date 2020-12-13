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

#include "httpparser.h"
#include "httpstatus.h"

#include <cstring>

namespace ws{
    void HttpParser::Again_Parser(){
        // TODO 这里不应该清空User_Buffer_，因为可能上一次收包只收了一部分；
        User_Buffer_->Clean();
        Parser_Result.release();
        Parser_Result = std::make_unique<HttpParser_Content>();
    }

    bool HttpParser::SetRequesting(){ 

        Request_Result->Set_VMajor(Parser_Result->V_major);
        Request_Result->Set_VMinor(Parser_Result->V_minor);

        //Request_Result->Set_CStart(Parser_Result);
        Request_Result->Set_CLength(Parser_Result->Content_length);
        Request_Result->Set_Method(Parser_Result->method);
        Request_Result->Set_Flag(Parser_Result->Set_Ka);    // keepalive还是close
        //Request_Result->Set_StatusCode(Parser_Result->Status);

        Request_Result->Set_Fault(Parser_Result->Fault);
        if(Parser_Result->Uri != nullptr){
            Request_Result->Set_Uri({Parser_Result->Uri, Parser_Result->Uri_length});
        }
        
        Request_Result->Set_Request_Buffer(User_Buffer_);

        return true;
    }

    HttpParserFault HttpParser::Starting_Parser(){
        if(!Parser_able()){
            // 缓冲区内数据太少，无法成功解析
            // TODO 这里应该直接退出的
            Parser_Result->Fault = HPFToLittleMessage;
        }
        
        Parsering();
        //std::cout << *Parser_Result << std::endl; //TODO : For debugging

        //TODO 
/*         if(Parser_Result->Fault == HPFContent){
            if(Parser_Result->Content_length != User_Buffer_->Readable() - 1)
                Parser_Result->Fault = HPFContent_Nonatch_Length;
        } */

        // 把Parser_Result中的一些必要的状态转移到Request_Result中
        SetRequesting();
        return Parser_Result->Fault;
    }

//                            _ooOoo_  
//                           o8888888o  
//                           88" . "88  
//                           (| -_- |)  
//                            O\ = /O  
//                        ____/`---'\____  
//                      .   ' \\| |// `.  
//                       / \\||| : |||// \  
//                     / _||||| -:- |||||- \  
//                       | | \\\ - /// | |  
//                     | \_| ''\---/'' | |  
//                      \ .-\__ `-` ___/-. /  
//                   ___`. .' /--.--\ `. . __  
//                ."" '< `.___\_<|>_/___.' >'"".  
//               | | : `- \`.;`\ _ /`;.`/ - ` : | |  
//                 \ \ `-. \_ __\ /__ _/ .-` / /  
//         ======`-.____`-.___\_____/___.-`____.-'======  
//                            `=---='  
//         .............................................  
//                  佛祖保佑             永无BUG 

    // 执行完Parsering以后如果解析成功请求头已经存储在Request_Result中了，其中状态机状态和解析结果存在Parser_Result中；
    bool HttpParser::Parsering(){
        static constexpr char CR = '\r';    // 回车
        static constexpr char LF = '\n';    // 换行
        static constexpr size_t BufferSize = 7;
        static constexpr char* ContentLength_key = (char*)"Content-Length";
        static constexpr char* ContentType_key = (char*)"Content-Type";
        static constexpr char* ContentLanguage = (char*)"Content-Language";
        static constexpr char* Connection_key = (char*)"Connection";
        static constexpr char* Keep_alive_value = (char*)"Keep-Alive";
        static constexpr char* Close_value = (char*)"Close";
        char Buffer[BufferSize];


        #define If_Conversion(cond, nextstatus) if(cond) {Parser_Result->Status = nextstatus; break; }
        #define If_Con_Exe(cond, nextstatus, stm) if(cond) {Parser_Result->Status = nextstatus; {stm}; break;}
        #define C_If_Con_Exe(cond, nextstatus, stm) if(cond) {Parser_Result->Status = nextstatus; {stm}; continue;}
        #define Set_Fault(fault) {Parser_Result->Status = HPSGAMEOVER; Parser_Result->Fault = fault; break;}

        // Parser_Result->Status初始状态为HPSOK
        while(Parser_Result->Status != HPSGAMEOVER && User_Buffer_->Readable()){
            const char ch = User_Buffer_->Peek(0);  // 其实也就是去取buffer中第一个可读的字符
            switch (Parser_Result->Status)
            {
                case HPSOK:
                    If_Conversion(ch == 'G', HPSGET);    // GET
                    If_Conversion(ch == 'P', HPSPOST);   // POST
                    If_Conversion(ch == 'O', HPSOPTION); // OPTION
                    If_Conversion(ch == 'H', HPSHEAD);   // HEAD
                    If_Conversion(ch == 'D', HPSDELETE); // DELETE
                    If_Conversion(ch == ' ', HPSOK);     // 空字符忽略
                    Set_Fault(HPFInvaildMethod);
                case HPSOPTION:
                    User_Buffer_->Read(Buffer,6);
                    C_If_Con_Exe(!memcmp(Buffer,"PTIONS",6),HPSBetweenMU,Parser_Result->method = HROptions;);
                    Set_Fault(HPFInvaildMethod);
                case HPSGET:
                    User_Buffer_->Read(Buffer,2);
                    C_If_Con_Exe(!memcmp(Buffer,"ET",2),HPSBetweenMU,Parser_Result->method = HRGet;);
                    Set_Fault(HPFInvaildMethod);
                case HPSHEAD:
                    User_Buffer_->Read(Buffer,3);
                    C_If_Con_Exe(!memcmp(Buffer,"EAD",3),HPSBetweenMU,Parser_Result->method = HRHead;);
                    Set_Fault(HPFInvaildMethod);
                case HPSDELETE: 
                    User_Buffer_->Read(Buffer,5);
                    C_If_Con_Exe(!memcmp(Buffer,"ELETE",5),HPSBetweenMU,Parser_Result->method = HRDELETE;);
                    Set_Fault(HPFInvaildMethod);
                case HPSPOST:
                    User_Buffer_->Read(Buffer,3);
                    C_If_Con_Exe(!memcmp(Buffer,"OST",3),HPSBetweenMU,Parser_Result->method = HRPost;);
                    Set_Fault(HPFInvaildMethod);
                
                case HPSBetweenMU:
                    If_Conversion(ch == ' ', HPSUriStart);
                    Set_Fault(HPFBetween_Method_URI_NoBlank);
                case HPSUriStart:
                    If_Conversion(ch == ' ', HPSUriStart);  // 忽略多余的空格
                    If_Con_Exe(isuri(ch), HPSURIParser, Parser_Result->Uri = User_Buffer_->ReadPtr(););
                    Set_Fault(HPFInvaildUri);
                case HPSURIParser:
                    If_Con_Exe(isuri(ch), HPSURIParser, ++(Parser_Result->Uri_length););
                    If_Conversion(ch == ' ', HPSUriEnd);
                    Set_Fault(HPFInvaildUri);
                case HPSUriEnd:
                    If_Conversion(ch == ' ', HPSUriEnd);    // 忽略多余的空格
                    User_Buffer_->Read(Buffer,5);
                    C_If_Con_Exe(!memcmp(Buffer,"HTTP/",5), HPSVersionMajor, ;);
                    Set_Fault(HPFInvaildVersion);
                case HPSVersionMajor:
                    If_Con_Exe(isdigit(ch), HPSVersionMajor, Parser_Result->V_major*=10;Parser_Result->V_major+=(ch-'0'););
                    If_Conversion(ch == '.', HPSVersionMinor);
                    Set_Fault(HPFInvaildVersion);
                case HPSVersionMinor:
                    If_Con_Exe(isdigit(ch), HPSVersionMinor, Parser_Result->V_minor*=10;Parser_Result->V_minor+=(ch-'0'););
                    If_Conversion(ch == CR, HPSVersionEnd); // 遇到CR以后http版本解析结束
                    Set_Fault(HPFInvaildVersion);
                case HPSVersionEnd:
                    If_Con_Exe(Parser_Result->V_major != 1 || (Parser_Result->V_minor != 1 && Parser_Result->V_minor != 0),
                    HPSGAMEOVER, Parser_Result->Fault = HPFNoSupportVersion;);
                    If_Conversion((ch == LF), HPSLF);
                    Set_Fault(HPFInvaildVersion);
                case HPSLF:
                    If_Conversion(ch == CR, HPSCRLFCR); // TODO 2020.12.12: 这地方以前解的有问题啊，换行只解了一个LR
                    If_Con_Exe(isheader(ch), HPSHeader, Parser_Result->Header = User_Buffer_->ReadPtr(););
                    Set_Fault(HPFInvaildHeader);
                case HPSCRLFCR:
                    // 解析结束的标记，两个CR+LF
                    If_Con_Exe(ch == LF, HPSGAMEOVER, Parser_Result->Fault = HPFContent;);
                    Set_Fault(HPFCRLFCR);
                case HPSHeader:
                    If_Con_Exe(isheader(ch), HPSHeader, ++(Parser_Result->Header_length););
                    If_Conversion(ch == ':', HPSColon);
                    Set_Fault(HPFInvaildHeader);
                case HPSColon:
                    If_Conversion(ch == ' ', HPSColon);
                    If_Con_Exe(isvalue(ch), HPSHeader_Value, Parser_Result->Value = User_Buffer_->ReadPtr(););
                    Set_Fault(HPFInvaildHeader);
                case HPSHeader_Value:
                    If_Con_Exe(isvalue(ch), HPSHeader_Value, ++(Parser_Result->Value_length););
                    If_Conversion(ch == CR, HPSStore_Header);
                    Set_Fault(HPFInvaildHeader_Value);
                case HPSStore_Header: 
                {
                    ParsedHeader _Header_{Parser_Result->Header, Parser_Result->Header_length};
                    ParsedHeader _Value_ {Parser_Result->Value, Parser_Result->Value_length};

                    // 请求头是存储在Request_Result中的
                    Request_Result->Store_Header(_Header_,_Value_);
                    Parser_Result->Header = nullptr;
                    Parser_Result->Header_length = 1;
                    Parser_Result->Value  = nullptr;
                    Parser_Result->Value_length = 1;

                    // 特判几种请求头
                    if(_Header_.ParsedHeaderIsEqual(Connection_key)){
                        if(_Value_.ParsedHeaderIsEqual(Keep_alive_value)){
                            Parser_Result->Set_Ka = Keep_Alive;
                        }else if(_Value_.ParsedHeaderIsEqual(Close_value)){
                            Parser_Result->Set_Ka = Close;
                        }else{
                            Set_Fault(HPFSetConnection);
                        }
                    }

                    if(_Header_.ParsedHeaderIsEqual(ContentLength_key)){
                        auto len = _Value_.Readable();
                        for(size_t i = 0; i < len; ++i){
                            if(isdigit(_Value_.Peek(i))){
                                Parser_Result->Content_length *= 10;
                                Parser_Result->Content_length += _Value_.Peek(i) - '0';
                            }else{
                                Set_Fault(HPFContentLength);
                            }
                        }
                    }
                    If_Conversion(true, HPSLF);
                }
                    default:
                        break;
            }

            User_Buffer_->read(1);
            // 当User_Buffer_缓冲区解析完毕且Extrabuffer_中还有数据时执行拷贝；
            if(User_Buffer_->Readable() == 0 && Extrabuffer_.IsVaild()){ //take out data from extrabuffer.
                User_Buffer_->Clean();
                auto length = Extrabuffer_.Get_length();
                if(length > User_Buffer_->Length()){
                    length = User_Buffer_->Length();
                }
                Extrabuffer_.Write(length);
                User_Buffer_->Write(Extrabuffer_.Get_ptr(), length);
            }
        }

        return true;
    }

#undef If_Conversion
#undef If_Con_Exe
#undef C_If_Con_Exe
#undef Set_Fault

}