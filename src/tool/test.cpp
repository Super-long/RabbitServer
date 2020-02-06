#include<bits/stdc++.h>
using namespace std;

int main(){
    unique_ptr<char[]> ptr(std::make_unique<char[]>(50));
    memset(ptr.get(), 0, 50);
    cout << "he\n";
};