
#ifndef GTEST_SAMPLE4_H
#define GTEST_SAMPLE4_H
//a simple monitonic counter
class Counter
{
private:   
    int counter_;
public:   
    //D'tor
    Counter():counter_(0){}
    //return the current counter value, and increments it.
    int Increment();
    //return the current counter value, and decrements it.
    int Decrement();
    //Prints the current counter value to STDOUT.
    void Print() const;
};
#endif GTEST_SAMPLE4_H