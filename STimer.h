//
// Created by LAD on 2023/11/6.
//

#ifndef UNTITLED2_STIMER_H
#define UNTITLED2_STIMER_H

#include <chrono>
#include <iostream>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <atomic>
#include <unistd.h>
class STimer {
public:
    STimer():m_periord(0),m_repeat(-1),m_active(false) {}
    template<typename Func,typename... Args>
    void SetTimer(int m_periord, Func&& func,Args&&... args);
    template<typename Func,typename Class,typename... Args>
    void SetTimer(int m_periord,Func&& func,Class&& c,Args... args);
    ~STimer();
    inline void Start();
    std::thread m_timethread;
    std::mutex m_mutex;
    std::condition_variable time_con_var;
    std::atomic<bool> m_active;
    std::function<void()> m_taskFunc;
    inline void TaskProcess(int milliseconds);
    int m_periord;
    int m_repeat;
};
inline void STimer::TaskProcess(int milliseconds) {
    while (this->m_repeat==-1||this->m_repeat>0){
        std::unique_lock<std::mutex> lck(this->m_mutex);
        this->time_con_var.wait_for(lck,std::chrono::milliseconds (this->m_periord));
        if(!this->m_active.load()){
            return;
        }
        if(this->m_repeat!=-1)
            this->m_repeat--;
        this->m_taskFunc();
    }
}
inline void STimer::Start() {
    if(this->m_active.load()){
         this->m_timethread=std::thread(&STimer::TaskProcess,this,this->m_periord);
//         this->m_timethread.detach();
    }
}
template<typename Func,typename Class,typename... Args>
void STimer::SetTimer(int m_periord, Func &&func, Class &&c, Args ...args) {
    static_assert(std::is_invocable_v<Func,Class, Args...>,"Invalid function signature");
    this->m_active.store(true);
    this->m_periord=m_periord;
    this->m_taskFunc=std::bind(std::forward<Func>(func),std::forward<Class>(c),std::forward<Args>(args)...);
}
template<typename Func,typename... Args>
void STimer::SetTimer(int m_periord, Func &&func, Args&&... args) {
    static_assert(std::is_invocable_v<Func, Args...>,"Invalid function signature");
    this->m_active.store(true);
    this->m_periord=m_periord;
    this->m_taskFunc=std::bind(std::forward<Func>(func),std::forward<Args>(args)...);
}


#endif //UNTITLED2_STIMER_H
