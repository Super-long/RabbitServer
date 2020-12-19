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

#include <liburing.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../base/nocopy.h"
#include "../base/havefd.h"

namespace ws{

/**
 * 用io_uring最大的问题是如何用接口接收网络套接字？
 * 如果监听ring_fd，就不能用epoll（浪费），直接poll或者select就可以,但是这需要每次都把事件关闭再加入；
 * 但是监听ring_fd我们只能判断一次来了几个事件，问题是没办法判断事件的类型，这意味着没办法判断套接字何时关闭；
 * 而且目前liburing这个库并不稳定，网上使用它的资料也可以说寥寥无几，所以暂且不急着引入；
*/

class IOUring : public Nocopy, public Havefd {
    private:
        struct io_data{    // 放在uring中的数据
            off_t offset;
            struct iovec iov;
            int fd;

            io_data() : offset(0), iov(), fd(-1){}
        };
    public:
        IOUring() : offset(0), SubmitdTaskNumber(0){
            io_uring_queue_init(queLength, &ring, 0);   // 后面的参数很难搞，用了竟然出现段错误
        }

        bool SubmitReadTask(void* array, size_t len, int fd, off_t offset = 0);

        bool Sumbit();

        static void queue_prepped(struct io_uring *ring, struct io_data *data){
            struct io_uring_sqe *sqe;

            sqe = io_uring_get_sqe(ring);
            assert(sqe);

            io_uring_prep_readv(sqe, data->fd, &data->iov, 1, data->offset);

            io_uring_sqe_set_data(sqe, data);
        }

        int NoBlockingWait();

        void DeleteIoData(){    // 很容易出现内存泄露，可以由外界管理io_data,在关闭连接的时候删除
            //struct io_data *data;
            //data = static_cast<struct io_data*>(io_uring_cqe_get_data(cqe));
            //delete data;
        }

        int fd() const & noexcept override {return ring.ring_fd;}

        ~IOUring(){
            io_uring_queue_exit(&ring);
        }

    private:

        struct io_uring ring;
        static const int queLength = 1024;
        uint32_t offset;
        uint32_t SubmitdTaskNumber;
};

}