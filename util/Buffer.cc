#include "Buffer.h"
#include <assert.h>
#include <memory>
#include <iostream>
#include <utility>
#include <cstring>
#include <numeric>
#include <limits>
namespace mrpc
{

inline size_t Roundup2power(size_t size_)
{
    if(size_ == 0)
        return 0;
    size_t new_size = 1;
    while(new_size < size_)
        new_size *= 2;
    return new_size;
}

const size_t Buffer::Kmaxbuffersize = std::numeric_limits<size_t>::max()/2;
const size_t Buffer::Kdefaultsize = 256;

Buffer& Buffer::_Movefrom(Buffer&& other)
{
    if(this == &other)
        return *this;
    readpos_ = other.readpos_;
    writepos_ = other.writepos_;
    capacity_ = other.capacity_;
    buffer_ = std::move(other.buffer_);

    return *this;
}

void Buffer::Shrink()
{
    if(Isempty())
    {
        if(capacity_ > 8*1024)
        {
            Clear();
            capacity_ = 0;
            buffer_.reset();
        }
        return;
    }

    size_t readsize = readablesize();
    size_t writesize = writablesize();
    if(readsize > capacity_/4)
        return;
    size_t new_cap = Roundup2power(readsize);
    unique_ptr<char[]> new_buffer(new char[new_cap]);

    buffer_.swap(new_buffer);
    capacity_ = new_cap;
    readpos_ = 0;
    writepos_ = readsize;
    return;
}

void Buffer::Swap(Buffer& other)
{
    std::swap(readpos_, other.readpos_);
    std::swap(writepos_, other.writepos_);
    std::swap(capacity_, other.capacity_);
    buffer_.swap(other.buffer_);
    return;
}

void Buffer::Assurespace(size_t needsize)
{
    if(writablesize() > needsize)
        return;
    size_t pre_cap = capacity_;
    while(writablesize()+readpos_ < needsize)
    {
        if(capacity_ <= Kdefaultsize)
        {
            capacity_ = Kdefaultsize;
        }
        else if(capacity_ <= Kmaxbuffersize)
        {
            size_t new_capacity_ = Roundup2power(pre_cap);
            if(new_capacity_ > capacity_)
                capacity_ = new_capacity_;
            else
                capacity_ = new_capacity_*3/2;
        }
        else
        {
            assert(false);
        }
    }
    if(capacity_ == pre_cap)
    {
        memmove(&buffer_[0], &buffer_[readpos_], readablesize());
        readpos_ = 0;
        writepos_ = readablesize();
    }
    else
    {
        unique_ptr<char[]> new_buffer_(new char[capacity_]);
        memcpy(&new_buffer_[0], &buffer_[readpos_], readablesize());
        readpos_ = 0;
        writepos_ = readablesize();
        buffer_.swap(new_buffer_);
    }
    return;
}

void Buffer::Clear()
{
    readpos_ = writepos_ = 0;
    return;
}

size_t Buffer::PushDataAt(const void* data, size_t len, size_t offset)
{
    if(!data || len == 0)
        return 0;
    if(offset + len + readablesize() > Kmaxbuffersize)
        return 0;
    Assurespace(len + offset);
    assert(offset + len <= writablesize());
    ::memcpy(&buffer_[writepos_+offset], data, len);
    return len;
}

size_t Buffer::PushData(const void* data, size_t len)
{
    size_t len_ = PushDataAt(data, len);
    Produce(len_);
    assert(len_ == len);
    return len;
}

size_t Buffer::PopData(void* des, size_t len)
{
    size_t len_ = PeekDataAt(des, len);
    Consume(len_);  //adjust readpos_ and writepos_
    return len_;
}
//PeekDataAt doesn't adjust readpos_ and writepos_
size_t Buffer::PeekDataAt(void* des, size_t len, size_t offset)
{
    size_t readsize = readablesize();
    if(!des || offset > readsize || len == 0)
        return 0;
//if the size is bigger than readable size, truncate it
    if(len+offset > readsize)
        len = readsize - offset;
    memcpy(des, &buffer_[readpos_+offset], len);
    return len;
}

void Buffer::Consume(size_t len)
{
    assert(readpos_ + len <= writepos_);
    readpos_ += len;
    if(Isempty())
        Clear();
    return;
}

void Buffer::Produce(size_t len)
{
    assert(writepos_ + len <= capacity_);
    writepos_ += len;
    return;
}
}
//namesapce mrpc





