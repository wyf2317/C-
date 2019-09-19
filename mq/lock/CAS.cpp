//  g++ -lpthread -std=c++11 thread.cpp 

#include <stdio.h>
#include <iostream>
#include <thread>
#include <atomic> 
#include <stdatomic.h>
int money;

void addMoney(int id)
{
    for (int i = 0; i < 10000; i++)
    {        
        int nOld = money;
        int nNew = old + 1;
        while (!atomic_fetch_add(&old, new))
        {
            nOld = money;
            nNew = old + 1;
        }
        money = money + 1;   
        
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