#include "time_chrono.h"
#include <chrono>
#include <functional>
#include <iostream>

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
void WithTime(function<void()> func, const char* PreInfo) {
    auto beginTime = std::chrono::high_resolution_clock::now();
    func(); // 调用传入的函数
    auto endTime = std::chrono::high_resolution_clock::now();
    auto timeInterval = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime);
    cout << PreInfo << " Execution time: " << timeInterval.count() << " ms" << endl;
}

// TimeKeeper
TimeKeeper::TimeKeeper() : startTime(std::chrono::high_resolution_clock::now()) {}

TimeKeeper::~TimeKeeper() {}

void TimeKeeper::Init()
{
    startTime = std::chrono::high_resolution_clock::now();
}

void TimeKeeper::dur() const
{
    auto endTime = std::chrono::high_resolution_clock::now();
    auto timeInterval = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << " dur time: " << timeInterval.count() << " ms" << std::endl;
}

void TimeKeeper::dur_withPre(const char* Pre) {
    printf("%-20s", Pre);
    TimeKeeper::dur();
}

void TimeKeeper::dur_withPre_Init(const char* Pre) {
    dur_withPre(Pre);
    Init();
}

// Watcher

Watcher::Watcher() : 
    Deep(0),DeepMaxLen(20),Level_Scripts(NULL),
    Time_Start(NULL){
    if (Time_Start == NULL) {
        DeepMaxLen = default_MaxDeep;
        Time_Start = new std::chrono::high_resolution_clock::time_point[default_MaxDeep];
        Level_Scripts = new const char* [default_MaxDeep];
        Time_Start[0] = std::chrono::high_resolution_clock::now();
    }
}

Watcher::Watcher(int DeepLen=20) : Deep(0) {
    if (Time_Start != NULL) {
        printf("Warring: Watcher DeepMaxLen be changed to %d\n", DeepLen);
    }
    DeepMaxLen = DeepLen;
    Time_Start = new std::chrono::high_resolution_clock::time_point[DeepMaxLen];
    Level_Scripts = new const char* [DeepMaxLen];
    Time_Start[0] = std::chrono::high_resolution_clock::now();
    Level_Scripts[0] = "Watcher Endddd";
}

int Watcher::GetDeep() {
    return Deep;
}

int Watcher::levelup(const char* Script) {
    Deep = Deep + 1;
    if (Deep > DeepMaxLen) {
        Deep = DeepMaxLen;
        printf("Warring: Deep level out of Max!\n");
        return 0;
    }
    Level_Scripts[Deep] = Script;
    Time_Start[Deep] = std::chrono::high_resolution_clock::now();
}

int Watcher::leveldown() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto timeInterval = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - Time_Start[Deep]);
    
    if (Deep < 0) {
        printf("Warring: Deep level out of Min!\n");
        return 0;
    }
    for (int i = 0; i < Deep-1; i++) {
        printf("|  ");
    }
    printf("|__%s : %lld ms\n",Level_Scripts[Deep],timeInterval);
    Deep = Deep - 1;
    
    return 1;
}

