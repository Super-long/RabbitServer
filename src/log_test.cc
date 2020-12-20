#include <unistd.h>
#include "log/logging.h"
#include "log/logfile.h"
#include "log/asynclogging.h"
#include <bits/stdc++.h>
using namespace std;

/**
 * 通过修改标准输出来达到导出logging的日志
 * @ 可优化
 * @ 1.logging对象改为单例 
 * @ 2.异步日志改为unordermap
 * @ 3.把所有的日志放到一个文件夹中
 * @ getlogfileName,获取线程号两点后来记得加
*/

std::unique_ptr<ws::detail::AsyncLogging> g_logFile;

void outputFunc(const char* msg, int len)
{
  g_logFile->append(msg, len);
}

/* void flushFunc()
{
  g_logFile->flush();
}
 */
/* int main(int argc, char* argv[])
{
  char name[256] = { 0 }; 
  strncpy(name, argv[0], sizeof name - 1);
  g_logFile.reset(new ws::detail::logfile(::basename(name), 200*1000));
  
  ws::detail::logging::setOutput(outputFunc);
  ws::detail::logging::setFlush(flushFunc); 

  std::string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
 
  for (int i = 0; i < 2000000; ++i)
  {
    ws::detail::log_INFO(__FILE__, __LINE__, errno).stream() << line << i << ":" << "\n";

    //usleep(1000);
  }
}
 */
int main(int argc, char* argv[])
{
  char name[256] = { 0 }; 
  strncpy(name, argv[0], sizeof name - 1);
  g_logFile.reset(new ws::detail::AsyncLogging(::basename(name), 200*1000));
  g_logFile->start(); // 开启异步线程

  std::string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
 
  for (int i = 0; i < 2000000; ++i)
  {
    ws::detail::log_INFO(__FILE__, __LINE__, errno).stream() << line << i << ":" << "\n";

    //usleep(1000);
  }
}