#include"httpstatus.h"

namespace ws{
    
    std::ostream& operator<<(std::ostream& os, const HttpParser_Content& para){
        using std::endl;
        os << "method : " << static_cast<int>(para.method) << endl;
        os << "Status : " << static_cast<int>(para.Status) << endl;
        os << "Fault  : " << static_cast<int>(para.Fault) << endl;
        os << "content_length : " << para.Content_length << endl;
        os << "V_major : " << para.V_major << endl;
        os << "V_minor : " << para.V_minor << endl;
        os << "Set_ka  : " << static_cast<int>(para.Set_Ka) << endl; 
        //os << "uri    : " << para.Uri << endl;
        os << "uri_length : " << para.Uri_length << endl;
        //For debugging
    }
}