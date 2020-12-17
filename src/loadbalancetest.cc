#include <bits/stdc++.h>
using namespace std;

// 对RabbitServer所采取的负载均衡策略的简单行为测试；

std::vector<uint32_t> PerThreadThroughput;
std::vector<long> CurrentWeight;
auto ThreadSum = 3;

int Distribution(){

    auto TotalWeight = std::accumulate(PerThreadThroughput.begin(), PerThreadThroughput.end(), 0); 

    for (size_t i = 0; i < CurrentWeight.size(); i++){
        CurrentWeight[i] += PerThreadThroughput[i];
    }
    size_t index = std::max_element(CurrentWeight.begin(), CurrentWeight.end()) - CurrentWeight.begin();
    CurrentWeight[index] -= TotalWeight;

    return index;
}

int main(){
    PerThreadThroughput.resize(ThreadSum);
    CurrentWeight.resize(ThreadSum, 0);

    for(auto& x : PerThreadThroughput){
        cin >> x;
    }

    vector<int> vec;
    vec.resize(ThreadSum);

    for (size_t i = 0; i < 10000; i++){
        vec[Distribution()]++;
    }

    vector<pair<uint32_t, size_t>> temp;
    for (size_t i = 0; i < ThreadSum; i++){
        temp.emplace_back(make_pair(vec[i], i));
    }
    sort(temp.begin(), temp.end());

    for (size_t i = 0; i < ThreadSum/2; i++){
        swap(vec[temp[i].second], vec[temp[ThreadSum - i - 1].second]);
    }

    for(auto x : vec){
        cout << x << endl;
    }

    return 0;
}
// 1000 2000 3000 4000 5000 6000 7000 8000 9000 10000 
// 3984500 3678000 4291000