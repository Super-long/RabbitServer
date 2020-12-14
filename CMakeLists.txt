cmake_minimum_required(VERSION 3.4.1)
project(Web_Server) #引入两个变量

set(CMAKE_CXX_STANDARD 14)
link_libraries(-pthread -lboost_system -lboost_thread)
link_libraries(-lprofiler) # 用于性能测试
SET(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS} -g -fno-strict-aliasing -O2 -Wall -Wextra -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 -Wnonnull)  #https://gcc.gnu.org/onlinedocs/gcc-4.4.7/gcc/Warning-Options.html
#add_definitions("-03")#下去搞清楚这个句子的含义 

aux_source_directory(src/base WS_BASE)
aux_source_directory(src/server WS_SERVER)
aux_source_directory(src/http/provider WS_PROVIDER)
aux_source_directory(src/FastCgi WS_FASTCGI)
aux_source_directory(src/net  WS_NET)
aux_source_directory(src/http WS_HTTP)
aux_source_directory(src/tool WS_TOOL)
aux_source_directory(src/log WS_LOG)
#搜索目录下所有的cpp文件

add_executable(Web_Server
        src/Web_Server.cc
        ${WS_SERVER}
        ${WS_PROVIDER}
        ${WS_BASE} #确实存在编译依赖的问题 这个放在上面在ws.cc中就出现了未定义的引用
        ${WS_NET}
        ${WS_HTTP}
        ${WS_TOOL}
        ${WS_FASTCGI}
        ${WS_LOG}
)