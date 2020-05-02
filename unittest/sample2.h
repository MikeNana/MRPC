#ifndef GTEST_SAMPLE2_H
#define GTEST_SAMPLE2_H

#include <string.h>

namespace
{
//a simple string class
class MyString
{
private:
    const char* c_string_;
    const MyString& operator= (const MyString& rhs);
public:
    static const char* CloneCString(const char* a_c_string);
    MyString():c_string_(nullptr){};
    explicit MyString (const char* a_c_string):c_string_(nullptr)
    {
        Set(a_c_string);
    }
    ~MyString()
    {
        delete []c_string_;
    }
    const char* c_string() const
    {
        return c_string_;
    }
    size_t Length() const
    {
        return c_string_ == nullptr ? 0 : strlen(c_string_);
    }
    void Set(const char* c_string);
};
}


#endif GTEST_SAMPLE2_H
