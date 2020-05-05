#include <cstring>  
#include "TimeUtil.h"

namespace mrpc
{

Time::Time():valid_(false)
{
    this->Now();
}

void Time::Now()
{
    now_ = std::chrono::system_clock::now();
    valid_ = false; 
}

int64_t Time::Millseconds() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(now_.time_since_epoch()).count(); 
}

int64_t Time::Microseconds() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>(now_.time_since_epoch()).count();
}

void Time::_UpdateTm() const
{
    if(valid_)
        return;
    valid_ = true;
    const time_t now(Millseconds() / 1000UL);
    ::localtime_r(&now, &tm_);
}

std::once_flag Time::init_;

const char* YEAR[] = 
{
    "2015", "2016", "2017", "2018", "2019",
    "2020", "2021", "2022", "2023", "2024",
    "2025", "2026", "2027", "2028", "2029",
    "2030", "2031", "2032", "2033", "2034",
    "2035", "2036", "2037", "2038", "2039",
    "2040", "2041", "2042", "2043", "2044",
    "2045", "2046", "2047", "2048", "2049",
    nullptr,
};

char Time::NUMBER[60][2] = {""};

void Time::Init()
{
    for (size_t i = 0; i < sizeof NUMBER / sizeof NUMBER[0]; ++i) 
    {
        char tmp[3];
        snprintf(tmp, 3, "%02d", static_cast<int>(i));
        memcpy(NUMBER[i], tmp, 2);
    }
}

std::size_t Time::FormatTime(char* buf) const
{
    std::call_once(init_, &Time::Init);     //ensure that Init will be called just once

    _UpdateTm();

    memcpy(buf, YEAR[tm_.tm_year + 1900 - 2020], 4);
    buf[4] = '-';
    memcpy(buf + 5, NUMBER[tm_.tm_mon + 1], 2);
    buf[7] = '-';
    memcpy(buf + 8, NUMBER[tm_.tm_mday], 2);
    buf[10] = '[';
    memcpy(buf + 11, NUMBER[tm_.tm_hour], 2);
    buf[13] = ':';
    memcpy(buf + 14, NUMBER[tm_.tm_min], 2);
    buf[16] = ':';
    memcpy(buf + 17, NUMBER[tm_.tm_sec], 2);
    buf[19] = '.';
    auto Msec = Microseconds();
    snprintf(buf + 20, 8, "%06d]", static_cast<int>(Msec % 1000000));

    return 27;
}

}
//end namespace mrpc