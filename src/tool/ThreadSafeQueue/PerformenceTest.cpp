#include <bits/stdc++.h>
#include "basequeue.h"
#include "effectivequeue.h"
#include "lockfreequeue.h"
#include <unistd.h>

const int ThreadNumber = 10;

struct Node{
    size_t son;
    size_t father;

    bool operator==(const Node& other) const {
        if(son == other.son && father == other.father) return true;
        return false;
    }

    Node() : son(0), father(0) {}

    Node(int x, int y) : son(x), father(y) {}
};

ws::BaseQueue<Node> que1;
void BaseQueueTest_Push(){
    for (size_t i = 0; i < 100000; i++){
        Node temp(i, i + 1);
        que1.push(temp);
    }
}

void BaseQueueTest_Pop(){
    ssize_t count = 0;
    for (size_t i = 0; i < 100000; i++){
        auto item = que1.pop();
        if(item == nullptr){
            count++;
        }
    }
    std::cout << count << std::endl;
}

ws::EffectiveQueue<Node> que2;
void EffectiveQueueTest_Push(){
    for (size_t i = 0; i < 100000; i++){
        Node temp(i, i + 1);
        que2.push(temp);
    }
}

void EffectiveQueueTest_Pop(){
    ssize_t count = 0;
    for (size_t i = 0; i < 100000; i++){
        auto item = que2.pop();
        if(item == nullptr){
            count++;
        }
    }
    std::cout << count << std::endl;
}

ws::LockFreeQueue<Node> que3;
void LockFreeQueueTest_Push(){
    for (size_t i = 0; i < 100000; i++){
        Node temp(i, i + 1);
        que3.push(temp);
    }
}

void LockFreeQueueTest_Pop(){
    ssize_t count = 0;
    for (size_t i = 0; i < 100000; i++){
        auto item = que3.pop();
        if(item == Node()){
            count++;
        }
    }
    std::cout << count << std::endl;
}

int main(){
    
    // BaseQueueTest
    auto start = std::chrono::high_resolution_clock::now(); 

    std::vector<std::thread> Threads(ThreadNumber);

    for (int i = 0; i < ThreadNumber; i++){
        if(i&1){
            Threads[i] = std::thread(BaseQueueTest_Push);
        } else {
            Threads[i] = std::thread(BaseQueueTest_Pop);
        }
    }

    std::for_each(Threads.begin(), Threads.end(), std::mem_fn(&std::thread::join));
    std::cout << "剩余总数 ：" << que1.size() << std::endl;

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::ratio<1,1000>> time_span 
    = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1,1000>>>(end - start);

    std::cout << "BaseQueue : " << time_span.count() << std::endl;

    // EffectiveQueueTest
    start = std::chrono::high_resolution_clock::now(); 

    for (int i = 0; i < ThreadNumber; i++){
        if(i&1){
            Threads[i] = std::thread(EffectiveQueueTest_Push);
        } else {
            Threads[i] = std::thread(EffectiveQueueTest_Pop);
        }
    }

    std::for_each(Threads.begin(), Threads.end(), std::mem_fn(&std::thread::join));
    std::cout << "剩余总数 ：" << que2.size() << std::endl;

    end = std::chrono::high_resolution_clock::now();

    time_span = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1,1000>>>(end - start);

    std::cout << "EffectiveQueue : " << time_span.count() << std::endl;

    // LockFreeQueueTest
    start = std::chrono::high_resolution_clock::now(); 

    for (int i = 0; i < ThreadNumber; i++){
        if(i<ThreadNumber/2){
            Threads[i] = std::thread(LockFreeQueueTest_Push);
        } else {
            //sleep(0.1);
            
            Threads[i] = std::thread(LockFreeQueueTest_Pop);
        }
    }

    std::for_each(Threads.begin(), Threads.end(), std::mem_fn(&std::thread::join));

    std::cout << "剩余总数 ：" << que3.size() << std::endl;

    end = std::chrono::high_resolution_clock::now();

    time_span = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1,1000>>>(end - start);

    std::cout << "LockFreeQueue : " << time_span.count() << std::endl;
}

// g++ PerformenceTest.cpp -pthread; ./a.out; rm a.out;