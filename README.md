# 介绍

RabbitServer是一个运行于Linux平台的HTTP服务器，采用C++14编写，目前支持处理HTTP/1.1的静态GET，HEAD，OPTIONS请求以及FastCGI请求。

# 特点
1. Reactor风格
2. 使用epoll边缘触发的IO多路复用技术
3. 使用独创的**无锁事件分发模型**
4. 采用**Timerfd**支持以事件驱动的多轮盘**Timewheel**，以实现应用层TCP保活机制
5. 基于**双缓冲区技术**与细粒度锁实现的**线程安全hashmap**实现了简单的异步日志系统
6. 大量利用析构函数，智能指针等手段实现**RAII**机制，无内存泄露
7. 实现基于状态机的HTTP解析
8. 实现**Fastcgi**协议与后台运行的php-fpm进行通信
9. 使用非阻塞connect实现简单客户端,支持**用户态重连**
10. 实现安全高效的用户态**InputBuffer**与**Ouputbuffer**，以保证服务器的高可用性
11. 解决**busy_loop**问题
12. 为保证高性能在发送文件时使用**零拷贝**技术
13. 利用**绑核机制**最大化CPU缓存命中率
14. 运行简单，直接在后台执行可以提供持续稳定的Web服务

# 安装与运行
运行Rabbitserver之前首先要确保你的机器已经正确配置了Cmake，你可以在软件源中下载并安装Cmake。
```
# debian && ubuntu
sudo apt-get install cmake

# arch
sudo pacman -S cmake
```

确保你的机器安装GCC，并支持c++14（最低6.0版本）你可以点击[这里](https://gcc.gnu.org/projects/cxx-status.html)查看GCC对C++特性的支持。

```
# arch
sudo pacman -S gcc
```
其他版本Linux比较繁琐，具体操作点击
[传送门](https://blog.csdn.net/f2157120/article/details/102830685?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522160785007719724813235689%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=160785007719724813235689&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduend~default-4-102830685.first_rank_v2_pc_rank_v29&utm_term=linux%E5%AE%89%E8%A3%85gcc&spm=1018.2118.3001.4449)


最后确保你的机器已经预安装了boost库。
```
wget http://sourceforge.net/projects/boost/files/boost/1.54.0/boost_1_54_0.tar.gz
tar -xzvf boost_1_54_0.tar.gz
cd boost_1_54_0
./bootstrap.sh --prefix=/usr/local
sudo ./b2 install --with=all
```

以上步骤完成以后我们就可以运行RabbitServer了。
```
mkdir build;
cd build;
cmake ..;
make;
./Web_Server;
```

# 测试环境
| 环境名称 | 值 | 
:-----:|:-----:|
系统|5.9.6-arch1-1|
处理器|4 x Intel® Core i5-7200U CPU @ 2.50GHz |
L1 Cache|32KB|
逻辑核数|4|
系统负载|3.07, 2.11, 1.99|
内存占用|5700MB(used)/1824MB(buff/cache)/215MB(free)
Swap |4012MB(used)/1014MB(avail mem)/4179MB(free)
Cmake版本|3.18.4|
GCC版本|10.2.0|

