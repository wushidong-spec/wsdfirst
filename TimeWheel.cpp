//
// Created by LAD on 2023/11/29.
//
//TimeWheel.cpp
#include "TimeWheel.h"
#if (_LEVEL <= _MAX_LEVEL)
unique_ptr<TimeWheel> TimeWheel::_me;

//初始化数据(tick: 滴答, grooveNum: 槽数)
void TimeWheel::init(UINT tick, array<USHORT, _LEVEL>& grooveNum)
{
    _tick = tick;
    //初始化槽/各层级槽数
    for (int i = 0; i < _LEVEL; _eventList[i] = new list<_timerInfo>[grooveNum[i]], _grooveNum[i] = grooveNum[i], ++i);
    //启动时间轮
    auto loop = new thread([this]()
                           {
                               while (_isRun)
                               {
                                   unique_lock<mutex> lock{ _lock };
                                   do{
                                       _condition.wait(lock, [this] { return _taskNum.load() != 0; });
                                       //二次检测
                                   } while (_taskNum == 0);
                                   //滴答指针
                                   this_thread::sleep_for(chrono::milliseconds(_tick));
                                   //事件更新函数
                                   auto runTask = [this](UINT __index) {
                                       //cout << "下标" << __index << "  指针：" << _pointer[__index] << endl;
                                       auto& taskList = _eventList[__index][_pointer[__index]];
                                       if (!taskList.empty())
                                       {
                                           for (list<_timerInfo>::iterator it = taskList.begin(); it != taskList.end();)
                                           {
                                               //检测剩余时间
                                               BOOL surplusTime = false;
                                               for (int i = __index - 1; i >= 0; --i)
                                               {
                                                   if (it->time[i] > 0)
                                                   {
                                                       //更新事件时间及存在层级
                                                       surplusTime = true;
                                                       it->time[__index] = 0;
                                                       it->level = i;
                                                       break;
                                                   }
                                               }
                                               //存在剩余时间
                                               if (surplusTime)
                                               {
                                                   //加入事件列表
                                                   it->groove = it->time[it->level] + _pointer[it->level];
                                                   _eventList[it->level][it->groove].push_back(*it);
                                                   it = taskList.erase(it);
                                               }
                                                   //无剩余时间
                                               else
                                               {
#if (_MODE == _ASYNCHRONOUS_MULTITHREADING)
                                                   auto task = new thread([&](function<void()>&& func)
                                {
                                    cout << "执行函数" << endl;
                                    func();
                                }, it->func);
#else
                                                   it->func();
#endif
                                                   if (it->once)
                                                   {
                                                       cout << "删除迭代器" << endl;
                                                       it = taskList.erase(it);
                                                       --_taskNum;
                                                       //如果没有任务
                                                       if (_taskNum == 0)
                                                           return;
                                                   }
                                                   else
                                                       ++it;
                                               }
                                           }
                                       }
                                   };
                                   //执行函数
                                   auto runTaskFunc = [&](UINT _index)
                                   {
#if (_MODE == _ASYNCHRONOUS_SINGLE_THREAD || _MODE == _ASYNCHRONOUS_MULTITHREADING)
                                       auto task = new thread([&](UINT&& ___index)
                                                              {
                                                                  runTask(___index);
                                                              }, _index);
#else
                                       runTask(_index);
#endif
                                   };
                                   //更新指针
                                   UINT index = 0;
                                   function<void(UINT&, bool)> upadtePointer;
                                   upadtePointer = [&](UINT& __index, bool __runtasks)
                                   {
                                       if (__index > _LEVEL) return;
                                       if (_pointer[__index] < _grooveNum[__index])
                                       {
                                           ++_pointer[__index];
                                           //上级指针更新
                                           if (_pointer[__index] == _grooveNum[__index])
                                           {
                                               _pointer[__index++] = 0;
                                               runTaskFunc(__index);
                                               upadtePointer(__index, false);
                                               return;
                                           }
                                           if (__runtasks)
                                               runTaskFunc(__index);
                                       }
                                   };
                                   upadtePointer(index, true);
                               }
                           });
    _isInit = true;
}

//清除定时事件
void TimeWheel::clearTimer(list<_timerInfo>::iterator timer)
{
    if (!_isInit) throw runtime_error("未初始化");
    _eventList[timer->level][timer->groove].erase(timer);
    --_taskNum;
}
#endif