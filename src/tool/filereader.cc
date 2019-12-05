#include"filereader.h"
#include<sys/sendfile.h>
namespace ws{

    ssize_t FileReader::SendFile(int Socket_Fd){
        return sendfile(Socket_Fd, fd(), &offest,length - offest);
    }

}