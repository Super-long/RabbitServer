#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "../base/nocopy.h"
#include "../net/epoll.h"
#include "../net/address.h"
#include "../base/config.h"

namespace ws{

class Connection : public Nocopy{
private:
enum ConnectionState{
        kDisconnected,
        kConnecting,
        kConnected };

int retryDelayMs_; //重连间隔时长
Address ServerAddress;
ConnectionState states;
std::shared_ptr<Epoll> ClientEpoll;
Socket socket_;

static const int kMaxRetryDelayMs = 30*1000;

void SetConnectionState(ConnectionState state){states = state;}
void Connecting(const Socket& socket);
void retry(int fd);

int getSocketError(int sockfd);
bool isSelfConnect(int sockfd);
struct sockaddr_in6 getLocalAddr(int sockfd);
struct sockaddr_in6 getPeerAddr(int sockfd);

public:
    explicit Connection(std::shared_ptr<Epoll> ptr) //constexper?
        : retryDelayMs_(500),
          states(kDisconnected), 
          ClientEpoll(ptr), 
          socket_(-1),
          ServerAddress(Y_Dragon::MyIP(), Y_Dragon::MyPort()){}

    void Connect();
    void HandleWrite(int fd, const std::function<void(int)>& newConnectionCallback);
};

}

#endif //CONNECTION_H_