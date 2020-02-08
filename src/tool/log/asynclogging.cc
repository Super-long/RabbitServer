#include "asynclogging.h"
#include "logfile.h"
#include "timestamp.h"
#include <chrono>

namespace ws{

namespace detail{
AsyncLogging::AsyncLogging(const std::string& name,
                off_t rollsize,
                size_t flushInterval)
    : basename(name),
      rollsize_(rollsize),
      flushInterval_(flushInterval),
      running(false),
      currentBuffer_(new Buffer),
      nextBuffer_(new Buffer)
    {
      currentBuffer_->setZero();
      nextBuffer_->setZero();
      buffers_.reserve(16);
    }

void 
AsyncLogging::start(){
    running = true;
    thread_ = std::thread(&AsyncLogging::AsyncFunc, this);
}

void
AsyncLogging::stop(){
    running = false;
    cv.notify_one();
    thread_.join();
}

void 
AsyncLogging::append(const char* line, int len){
    std::lock_guard<std::mutex> guard(mutex_);
    if(currentBuffer_->avail() > len){
      currentBuffer_->append(line, len);
    }else{
      buffers_.push_back(std::move(currentBuffer_));
      if(nextBuffer_){
        currentBuffer_ = std::move(nextBuffer_);
      }else{
        currentBuffer_.reset(new Buffer);
      }
      currentBuffer_->append(line, len);
      cv.notify_one();
    }
}

void
AsyncLogging::AsyncFunc(){
    logfile ouput(basename, rollsize_, false);
    Bufferptr newBuffer1(new Buffer);
    Bufferptr newBuffer2(new Buffer);
    newBuffer1->setZero();
    newBuffer2->setZero();
    Buffervector WriteLog;
    WriteLog.reserve(16);
    auto interval = std::chrono::seconds(flushInterval_);

    while(running){
        {
          std::unique_lock<std::mutex> guard(mutex_);
          if(buffers_.empty()){
            cv.wait_for(guard, interval);
          }
          buffers_.push_back(std::move(currentBuffer_));
          currentBuffer_ = std::move(newBuffer1);
          WriteLog.swap(buffers_);
          if(!nextBuffer_){
            nextBuffer_ = std::move(newBuffer2);
          }
        }

        //处理内存堆积 日志占用过多内存
        if(WriteLog.size() > 25){
            char buf[256];
            snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
                     Timestamp::now().toFormattedString().c_str(),
                     buffersToWrite.size()-2);
            fputs(buf, stderr);
            output.append(buf, static_cast<int>(strlen(buf)));
            buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());//留两个缓冲块
        }

        for(const auto& buffer : WriteLog){
            output.append(buffer->data(), buffer->length());
            //writev 分块写入是要快 但是使用buffer的write也不慢
        }

        if(WriteLog.size() > 2){
          WriteLog.resize(2);
        }

        if(!newBuffer1){
          newBuffer1 = std::move(WriteLog.back());
          WriteLog.pop_back();
          newBuffer1->setSpotBegin();
        }

        if(!newBuffer2){
          newBuffer2 = std::move(WriteLog.back());
          WriteLog.pop_back();
          newBuffer2->setSpotBegin();
        }
        WriteLog.clear();
        ouput.flush();
    }
    ouput.flush();
}

}

}