// to be tested

#include <vector>
#include <cassert>
#include "Timer.h"

namespace mrpc
{
namespace internal
{

unsigned int TimerManager::s_timer_id_gen_ = 0;

TimerManager::TimerManager()
{}

TimerManager::~TimerManager()
{}

void TimerManager::Update()
{
    if(timers_.empty())
        return;
    
    const auto now = std::chrono::steady_clock::now();
    
    for(auto it(timers_.begin()); it != timers_.end(); )
    {
        if(it->first > now)
            return;
        
        //support cancel self
        it->second.OnTimer();

        //erase and replace the timer
        Timer timer(std::move(it->second));
        it = timers_.erase(it);
        if(timer.count_ != 0)
        {
            //need reschedule
            const auto tp = timer.id_->first;
            auto itNew = timers_.insert(std::make_pair(tp, std::move(timer)));
            if(it == timers_.end() || it->first > itNew->first)
                it = itNew;
        }
    }
}

bool TimerManager::Cancel(TimerId id)
{
    auto begin = timers_.lower_bound(id->first);
    if(begin == timers_.end())
        return false;

    auto end = timers_.upper_bound(id->first);
    for(auto it(begin); it != end; ++it)
    {
        if(it->second.UniqueId() == id->second)
        {
            it->second.count_ = 0;
            return true;
        }
    }

    return false;
}

DurationMs TimerManager::NearestTimer() const
{
    if(timers_.empty())
        return DurationMs::max();
    
    const auto& timer = timers_.begin()->second;
    auto now = std::chrono::steady_clock::now();
    if(now > timer.Id()->first)
        return DurationMs::min();
    else
        return std::chrono::duration_cast<DurationMs>(timer.Id()->first - now);
}

TimerManager::Timer::Timer(const TimePoint& tp):
    id_(std::make_shared<std::pair<TimePoint, unsigned int>>(tp, ++ TimerManager::s_timer_id_gen_)), 
    count_(Kforever)
{}

void TimerManager::Timer::_Move(Timer&& timer)
{
    this->func_ = std::move(timer.func_);
    this->interval_ = std::move(timer.interval_);
    this->id_ = std::move(timer.id_);
    this->count_ = std::move(timer.count_);
}

TimerManager::Timer::Timer(Timer&& timer)
{
    _Move(std::move(timer));
}

TimerManager::Timer& TimerManager::Timer::operator= (Timer&& timer)
{
    if(this != &timer)
        _Move(std::move(timer));
    return *this;
}

void TimerManager::Timer::OnTimer()
{
    if(!func_ || count_ == 0)
        return;

    if(count_ == Kforever || count_-- > 0)
    {
        func_();
        id_->first += interval_;
    }
    else
    {
        count_ = 0;     //if count_ is equal to 0, keep it
    }
}

TimerId TimerManager::Timer::Id() const
{
    return id_;
}

unsigned int TimerManager::Timer::UniqueId() const
{
    return id_->second;
}


}//end namespace internal
}//end namespace mrpc


