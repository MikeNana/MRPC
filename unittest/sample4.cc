#include "sample4.h"
#include <iostream>

int Counter::Increment()
{
    return counter_++;
}

int Counter::Decrement()
{
    if(counter_ == 0)
        return 0;
    return counter_--;
}

void Counter::Print() const
{
    std::cout << counter_ << std::endl;
}