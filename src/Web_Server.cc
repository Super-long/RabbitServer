#include"server/ws.h"
#include <gperftools/profiler.h>    //https://zhuanlan.zhihu.com/p/259689535
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

// 用于服务端程序执行性能测试
void setGperfStatus(int signum) {
    static bool is_open = false;
    if (signum != SIGUSR1) {
        return ;
    }
    if (!is_open) {  // start
        is_open = true;
        ProfilerStart("test.prof");
        std::cout << "ProfilerStart success" << std::endl;
    } else {  // stop
        is_open = false;
        ProfilerStop();
        std::cout << "ProfilrerStop success" << std::endl;
    }
}

int main(int argc, char **argv){
    // https://www.gnu.org/software/libc/manual/html_node/Miscellaneous-Signals.html
    signal(SIGUSR1, setGperfStatus);
    std::cout << "pid = " << getpid() << std::endl; // 通过发送请求来开始和终止性能测试
    ws::Web_Server().Running();
    return 0;
}

/*
 * 执行性能测试步骤：
 * step0: 执行./Web_Server开启服务器
 * step1：开启一个终端，执行 kill -s SIGUSR1 $PID，PID为RabbitServer的服务进程，此时开始性能分析
 * step2：对RabbitServer执行压力测试，比如ab，执行 ab -n 100000 -c 100  127.0.0.1:8888/
 * step3：执行kill -s SIGUSR1 $PID，结束性能分析
 * step4：此时已经生成了test.prof, 执行 pprof ./Web_Server test.prof --pdf > test.pdf
 * */

 /**
 * 0. ApacheBench文档：http://httpd.apache.org/docs/2.4/programs/ab.html
 * ab文档不太全，Connection Times字段没有：https://stackoverflow.com/questions/2820306/definition-of-connect-processing-waiting-in-apache-bench
 * 1. 使用ApacheBench：https://www.howtoing.com/how-to-use-apachebench-to-do-load-testing-on-an-arch-linux-vps
 * 2. Apache安装文档：http://linux.it.net.cn/e/distro/archlinux/2016/0519/22053.html
 * 3. Nginx安装文档：https://wiki.archlinux.org/index.php/Nginx_(%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87)
 */

 /**
  * 如果没有配置的话Apache和Nginx默认端口均为80，所以需要关闭一个重启另外一个，当然配置以后就另说了；
  * 0. 启动Nginx服务：systemctl start nginx  
  * 1. 关闭Nginx服务：systemctl stop nginx
  * 2. 启动Apache服务：systemctl start httpd
  * 3. 关闭Apache服务：systemctl stop httpd 
 */