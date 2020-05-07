#ifndef TIMEUTIL_H_
#define TIMEUTIL_H_


//call_once(init_, &Time::Init);  init_(std::once_flag) ensures that Init will be called just once
//if the flag init_ is true, it means Init was called already, then call_once returns right away
//otherwise Init will be called right away.

#include <ctime>   
#include <chrono>   
#include <stdint.h>   //for int types
#include <mutex>   

namespace mrpc
{

//a brief encapsulation for time point
class Time
{
private:   
    std::chrono::system_clock::time_point now_;
    mutable bool valid_;
    mutable tm tm_;

    void _UpdateTm() const;
    //for format time
    static std::once_flag init_;
    static void Init();
    static const char* YEAR[];
    static char NUMBER[60][2];
public:   
    Time();
    void Now();
    int64_t Millseconds() const;
    int64_t Microseconds() const;
    std::size_t FormatTime(char* buf)const;

    int GetYear() const
    {
        _UpdateTm();
        return tm_.tm_year + 1900;
    }

    int GetMonth() const
    {
        _UpdateTm();
        return tm_.tm_mon + 1;
    }

    int GetDay() const
    {
        _UpdateTm();
        return tm_.tm_mday;
    }

    int GetHour() const
    {
        _UpdateTm();
        return tm_.tm_hour;
    }

    int GetMinute() const
    {
        _UpdateTm();
        return tm_.tm_min;
    }

    int GetSecond() const
    {
        _UpdateTm();
        return tm_.tm_sec;
    }
};


}

#endif 