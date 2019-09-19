//  g++ -lpthread -std=c++11 thread.cpp 

#include <stdio.h>
#include <iostream>
#include <thread>
#include <mutex>          // std::mutex

std::mutex mtx;           // mutex for critical section
int money;

void addMoney(int id)
{
    for (int i = 0; i < 10000; i++)
    {        
        mtx.lock();
        money = money + 1;   
        mtx.unlock();       
    }
}

int main()
{
    money = 0;
    std::thread threads[100];
    for (int i = 0 ; i < 100; i++)
    {
       threads[i] = std::thread(addMoney, i);
    }
    
    
    for (auto& th : threads) th.join();
    printf("total money:%d\n", money);
    return 0;
}