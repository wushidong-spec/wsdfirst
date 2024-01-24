//
// Created by LAD on 2023/11/29.
//

#ifndef UNTITLED2_TIMEWHEEL_H
#define UNTITLED2_TIMEWHEEL_H

//TimeWheel.h
#pragma once
#include <iostream>
#include <windows.h>
#include <array>
#include <list>
#include <atomic>
#include <thread>
#include <mutex>
#include <assert.h>
#include <functional>
#include <condition_variable>
using namespace std;

#define _SYNCHRONIZE 1                    //同步
#define _ASYNCHRONOUS_SINGLE_THREAD 2    //异步单线程
#define _ASYNCHRONOUS_MULTITHREADING 3    //异步多线程
//模式(自由选择)
#define _MODE _ASYNCHRONOUS_SINGLE_THREAD

#define _MAX_LEVEL 4                    //最大层级
#define _LEVEL 4                        //层级

#if (_LEVEL <= _MAX_LEVEL)
class TimeWheel final
{
private:
    //事件信息
    typedef struct _timerInfo
    {
        UINT level;                    //存在层级
        UINT groove;                //存在槽数
        bool once;                    //是否只执行一次
        function<void()> func;        //事件回调
        array<UINT, _LEVEL> time;    //事件层级(小到大)
    }TIMEINFO, * TIMEINFO_PTR;

    static unique_ptr<TimeWheel> _me;        //实例指针
    mutex _lock;                            //线程锁
    condition_variable _condition;            //条件变量
    array<USHORT, _LEVEL> _grooveNum;        //层级槽数
    array<USHORT, _LEVEL> _pointer{ 0 };    //层级指针
    UINT _tick;                                //滴答(单位:毫秒)
    list<_timerInfo>* _eventList[_LEVEL];    //事件列表
    atomic<UINT> _taskNum{ 0 };                //当前任务数
    atomic<bool> _isInit{ false };            //是否初始化
    atomic<bool> _isRun{ true };            //是否正在运行
protected:
    TimeWheel() = default;
    TimeWheel(CONST TimeWheel&) = delete;
    TimeWheel& operator =(CONST TimeWheel&) = delete;
public:
    ~TimeWheel() {
        _isRun = false;
    };
    //获取实例
    static TimeWheel& getInstance() {
        static once_flag _flag;
        call_once(_flag, [&]() {
            _me.reset(new TimeWheel);
        });
        return *_me;
    }
    //销毁
    void destroy() {
        _me.reset(nullptr);
    }

    //初始化数据(tick: 滴答, grooveNum: 槽数)
    void init(UINT tick, array<USHORT, _LEVEL>& grooveNum);
    //添加定时事件(time: 秒)
    template<typename Func, typename... Args>
    auto addTimer(FLOAT time, BOOL loop, Func&& func, Args&& ... args);
    //清除定时事件
    void clearTimer(list<_timerInfo>::iterator timer);
};

//添加定时事件
template<typename Func, typename... Args>
auto TimeWheel::addTimer(FLOAT time, BOOL loop, Func&& func, Args&& ... args)
{
    if (!_isInit) throw runtime_error("未初始化");
    //毫秒数
    UINT ms = 1000 * time;
    //最大时间单位
    char maxUnit = -1;
    TIMEINFO timerEvent;
    //计算各层级时间
    for (int i = _LEVEL-1; i > -1; --i)
    {
        //获取下一级时间单位
        UINT timeUnit = _tick;
        for (int j = 0; j < i; timeUnit *= _grooveNum[j++]);
        //计算轮数
        timerEvent.time[i] = ms / timeUnit;
        //更新剩余时间
        if (timerEvent.time[i] > 0)
        {
            ms -= timerEvent.time[i] * timeUnit;
            if (maxUnit == -1)
                maxUnit = i;
        }
    }
    //设置事件数据
    assert(maxUnit != -1);
    timerEvent.level = maxUnit;
    timerEvent.func = bind(forward<Func>(func), forward<Args>(args)...);
    timerEvent.once = loop;
    {
        lock_guard<mutex> lock{ _lock };
        timerEvent.groove = timerEvent.time[timerEvent.level] + _pointer[timerEvent.level];
        //加入事件列表
        _eventList[timerEvent.level][timerEvent.groove].emplace_back(timerEvent);
    }
    ++_taskNum;
    //唤醒线程
    _condition.notify_one();
    return --_eventList[timerEvent.level][timerEvent.groove].end();
}
#endif


#endif //UNTITLED2_TIMEWHEEL_H
