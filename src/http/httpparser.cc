#include"httpparser.h"
#include"httpstatus.h"
#include<cstring>

namespace ws{
    void HttpParser::Again_Parser(){
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
        Request_Result->Set_Flag(Parser_Result->Set_Ka);
        //Request_Result->Set_StatusCode(Parser_Result->Status);

        Request_Result->Set_Fault(Parser_Result->Fault);
        if(Parser_Result->Uri != nullptr)
        Request_Result->Set_Uri({Parser_Result->Uri, Parser_Result->Uri_length});
        Request_Result->Set_Request_Buffer(User_Buffer_);

    }

    HttpParserFault HttpParser::Starting_Parser(){
        if(!Parser_able()){
            Parser_Result->Fault = HPFToLittleMessage;
        }
        
        Parsering();
        //std::cout << *Parser_Result << std::endl; //TODO : For debugging

        //TODO 
/*         if(Parser_Result->Fault == HPFContent){
            if(Parser_Result->Content_length != User_Buffer_->Readable() - 1)
                Parser_Result->Fault = HPFContent_Nonatch_Length;
        } */
        SetRequesting();
        return Parser_Result->Fault;
    }

    bool HttpParser::Parsering(){
        static constexpr char CR = '\r';
        static constexpr char LF = '\n';
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


        while(Parser_Result->Status != HPSGAMEOVER && User_Buffer_->Readable()){
            const char ch = User_Buffer_->Peek(0);
            switch (Parser_Result->Status)
            {
                case HPSOK:
                    If_Conversion(ch == 'G',HPSGET);
                    If_Conversion(ch == 'P',HPSPOST);
                    If_Conversion(ch == 'O',HPSOPTION);
                    If_Conversion(ch == 'H',HPSHEAD);
                    If_Conversion(ch == 'D',HPSDELETE);
                    If_Conversion(ch == ' ',HPSOK);                    
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
                    If_Conversion(ch == ' ', HPSUriStart);
                    If_Con_Exe(isuri(ch),HPSURIParser, Parser_Result->Uri = User_Buffer_->ReadPtr(););
                    Set_Fault(HPFInvaildUri);
                case HPSURIParser:
                    If_Con_Exe(isuri(ch), HPSURIParser, ++(Parser_Result->Uri_length););
                    If_Conversion(ch == ' ', HPSUriEnd);
                    Set_Fault(HPFInvaildUri);
                case HPSUriEnd:
                    If_Conversion(ch == ' ', HPSUriEnd);
                    User_Buffer_->Read(Buffer,5);
                    C_If_Con_Exe(!memcmp(Buffer,"HTTP/",5), HPSVersionMajor, ;);
                    Set_Fault(HPFInvaildVersion);
                case HPSVersionMajor:
                    If_Con_Exe(isdigit(ch), HPSVersionMajor, Parser_Result->V_major*=10;Parser_Result->V_major+=(ch-'0'););
                    If_Conversion(ch == '.', HPSVersionMinor);
                    Set_Fault(HPFInvaildVersion);
                case HPSVersionMinor:
                    If_Con_Exe(isdigit(ch), HPSVersionMinor, Parser_Result->V_minor*=10;Parser_Result->V_minor+=(ch-'0'););
                    If_Conversion(ch == CR, HPSVersionEnd);
                    Set_Fault(HPFInvaildVersion);     
                case HPSVersionEnd:
                    If_Con_Exe(Parser_Result->V_major != 1 || (Parser_Result->V_minor != 1 && Parser_Result->V_minor != 0),
                    HPSGAMEOVER, Parser_Result->Fault = HPFNoSupportVersion;);
                    If_Conversion((ch == LF), HPSLF);
                    Set_Fault(HPFInvaildVersion);
                case HPSLF:
                    If_Conversion(ch == CR, HPSCRLFCR);
                    If_Con_Exe(isheader(ch), HPSHeader, Parser_Result->Header = User_Buffer_->ReadPtr(););
                    Set_Fault(HPFInvaildHeader);
                case HPSCRLFCR:
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
                    Request_Result->Store_Header(_Header_,_Value_);
                    Parser_Result->Header = nullptr;
                    Parser_Result->Header_length = 1;
                    Parser_Result->Value  = nullptr;
                    Parser_Result->Value_length =1;

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
        }

    }

#undef If_Conversion
#undef If_Con_Exe
#undef C_If_Con_Exe
#undef Set_Fault

}