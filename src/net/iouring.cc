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

#include "iouring.h"

namespace ws{

    bool IOUring::SubmitReadTask(void* array, size_t len, int fd, off_t offset){
            struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
            if(!sqe){
                return false;
            }

            struct io_data* data = new io_data();
            if (!data){
                return false;
            }
            
            data->iov.iov_base = array;
            data->iov.iov_len = len;
            data->fd = fd;
            data->offset = offset;

            ++SubmitdTaskNumber;

            io_uring_prep_readv(sqe, fd, &data->iov, 1, offset);
            io_uring_sqe_set_data(sqe, static_cast<void*>(data));
            return true;
    }

    bool IOUring::Sumbit(){
        int ret = io_uring_submit(&ring);
        if (ret < 0) {
            fprintf(stderr, "io_uring_submit: %s\n", strerror(-ret));
            return false;
        } else if (ret != SubmitdTaskNumber) {  // 提交事件小于放到sqe中的事件数
            fprintf(stderr, "io_uring_submit submitted less %d\n", ret);
            return false;
        }
        SubmitdTaskNumber = 0;
        return true;
    }

    /**
     * @notes: 返回成功读取的字节数，用于更新userbuffer;-1为失败
    */
    int IOUring::NoBlockingWait(){
        struct io_uring_cqe *cqe;
        struct io_data *data;

        int ret = io_uring_peek_cqe(&ring, &cqe); // 非阻塞等待

        if (ret < 0) {
            fprintf(stderr, "io_uring_peek_cqe: %s\n", strerror(-ret));
            return -1;
        }

        data = static_cast<struct io_data*>(io_uring_cqe_get_data(cqe));

        if (cqe->res < 0) {
            if (cqe->res == -EAGAIN || cqe->res == EWOULDBLOCK) {   // 返回值小于零但是错误类型为EAGAIN的时候可以重新进行
                queue_prepped(&ring, data);      // 重新插入
                io_uring_cqe_seen(&ring, cqe);   // 标记该事件已经被处理
                return -1;
            }
            fprintf(stderr, "cqe failed: %s\n",
                    strerror(-cqe->res));
            return -1;
        } else if (cqe->res != data->iov.iov_len) { // 没读全或者没写全，对套接字如何界定没读全的范围呢？
            data->iov.iov_base = (char*)data->iov.iov_base + cqe->res; // 转移写指针偏移量
            data->iov.iov_len -= cqe->res;
            data->offset += cqe->res;
            queue_prepped(&ring, data);
            io_uring_cqe_seen(&ring, cqe);
        }

        return cqe->res;
    }

}