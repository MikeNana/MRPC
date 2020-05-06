#ifndef TIMERMANAGER_H_
#define TIMERMANAGER_H_

#include <chrono>  
#include <memory>
#include <ostream>
#include <map>  
#include <functional>

namespace mrpc
{

using DurationMs = std::chrono::milliseconds;
using TimePoint = std::chrono::steady_clock::time_point;        //record program time
using TimerId = std::shared_ptr<std::pair<TimePoint, unsigned int>>; 

constexpr int Kforever = -1;

inline std::ostream& operator << (std::ostream& os, const TimerId& d)
{
    os << "[Timer Id:" << (void*)d.get() << "]";
    return os;
}

namespace internal
{
//TimerManager class; You should not use it directly, but via Eventloop
class TimerManager final
{
public:
    TimerManager();
    ~TimerManager();

    TimerManager(const TimerManager&) = delete;
    TimerManager& operator = (const TimerManager&) = delete;
    //Tick
    void Update();

    //function:schedule timer at absolute time point then repeat with period
    //triggerTime:the absolute time when timer first triggered
    //period:After first trigger, will be triggered by this period repeated until RepeatCount
    //f:function to execute
    //args:arguments for f   
    template<int RepeatCount, typename Duration, typename F, typename... Args>                                      
    TimerId ScheduleAtWithRepeat(const TimePoint& triggertime, const Duration& period, F&& f, Args&&... args);

    //PAY ATTENTION:not be triggered at once but after period
    template<int RepeatCount, typename Duration, typename F, typename... Args>
    TimerId ScheduleAfterWithRepeat(const Duration& period, F&& f, Args&&... args);

    //be triggered at triggertime, just once
    template<typename F, typename... Args>
    TimerId ScheduleAt(const TimePoint& triggertime, F&& f, Args&&... args);

    //be triggered after period, just once
    template<typename Duration, typename F, typename... Args>  
    TimerId ScheduleAfter(const Duration& period, F&& f, Args&&... args);

    //cancel timer
    bool Cancel(TimerId id);

    //how far the nearest timer will be triggered
    DurationMs NearestTimer() const;
private:
    class Timer
    {
        friend class TimerMananger;
    public:
        explicit Timer(const TimePoint& tp);
        //only move
        Timer(Timer&& timer);
        Timer& operator = (Timer&& timer);

        Timer(const Timer& timer) = delete;
        Timer& operator = (const Timer& ) = delete;

        template<typename F, typename... Args>
        void SetCallBack(F&& f, Args&&... args);

        void OnTimer();

        TimerId Id() const;
        unsigned int UniqueId() const;
    private:
        void _Move(Timer&& timer);

        TimerId id_;

        std::function<void()> func_;    
        DurationMs interval_;
        int count_;
    };

    std::multimap<TimePoint, Timer> timers_;
    friend class Timer;

    //not thread-safe, but why not care?
    static unsigned int s_timer_id_gen_;
};

template<int RepeatCount, typename Duration, typename F, typename... Args>
TimerId TimerManager::ScheduleAtWithRepeat(const TimePoint& triggertime, const Duration& period, F&& f, Args&&... args)
{
    static_assert(RepeatCount != 0, "Why you need a timer with a zero count?");

    using namespace std::chrono;

    Timer t(triggertime);
    //precision: millisecond
    t.interval_ = std::max(DurationMs(1), duration_cast<DurationMs>(period));
    t.count_ = RepeatCount;
    TimerId id = t.Id();

    t.SetCallBack(std::forward<F>(f), std::forward<Args>(args)...);
    timers_.insert(std::make_pair(triggertime, t));
    return id;
}

template<int RepeatCount, typename Duration, typename F, typename... Args>
TimerId TimerManager::ScheduleAfterWithRepeat(const Duration& period, F&& f, Args&&... args)
{
    const auto now = std::chrono::steady_clock::now();
    return ScheduleAtWithRepeat<RepeatCount>(now + period, period, std::forward<F>(f), std::forward<Args>(args)...);
}

template<typename F, typename... Args>
TimerId TimerManager::ScheduleAt(const TimePoint& triggertime, F&& f, Args&&... args)
{
    return ScheduleAtWithRepeat<1>(triggertime, DurationMs(0), std::forward<F>(f), std::forward<Args>(args)...);
}

template<typename Duration, typename F, typename... Args>
TimerId TimerManager::ScheduleAfter(const Duration& period, F&& f, Args&&... args)
{
    const auto now = std::chrono::steady_clock::now();
    return ScheduleAt(now + period, period, std::forward<F>(f), std::forward<Args>(args)...);
}

template<typename F, typename... Args>
void TimerManager::Timer::SetCallBack(F&& f, Args&&... args)
{
    func_ = std::bind(std::forward<F> (f), std::forward<Args> (args)...);
}
    
}   //end namespace internal

}   //end namespace mrpc

#endif