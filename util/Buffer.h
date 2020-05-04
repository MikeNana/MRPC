/*
主要用于实现和缓冲区相关的几个底层结构
slice
buffer:扩展规则和vector相同，并没有使用环形缓冲区，如果空间不够则roundup，如果空间太大则shrink
buffervector
*/
//ifndef用于防止多重定义，当多个文件编译时，同一个文件被多次包含
#ifndef BUFFER_H_
#define BUFFER_H_

#include <memory>
#include <list>
using std::unique_ptr;
namespace mrpc
{

class Buffer
{
private:    
    Buffer& _Movefrom(Buffer&& );

    size_t readpos_;
    size_t writepos_;
    size_t capacity_;
    unique_ptr<char[]> buffer_;

public:
    Buffer():
        readpos_(0), 
        writepos_(0), 
        capacity_(0)
        {}
    Buffer(const void* data, size_t size):
        readpos_(0),
        writepos_(0),
        capacity_(0)
        {
            PushData(data, size);
        }
//不允许Buffer进行基于左值的拷贝和复制操作
    void operator = (const Buffer&) = delete;
    Buffer(const Buffer&) = delete;
//可以进行右值转移的操作    
    void operator = (Buffer&&);
    Buffer(Buffer&&);
    
    size_t readablesize() const
    {
        if(writepos_ >= readpos_)
            return writepos_-readpos_;
        return 0;
    }

    size_t writablesize() const
    {
        if(writepos_ <= capacity_)
            return capacity_-writepos_;
        return 0;
    }

    size_t capacity() const
    {
        return capacity_;
    }
    char* readaddr()
    {
        return &buffer_[readpos_];
    }

    char* writeaddr()
    {
        return &buffer_[writepos_];
    }

    bool Isempty()
    {
        return readpos_ == writepos_;
    }
//数据的读取写入函数
    size_t PushDataAt(const void* data, size_t len, size_t offset = 0);
    size_t PushData(const void* data, size_t len);
    size_t PopData(void* des, size_t len);
    size_t PeekDataAt(void* des, size_t len, size_t offset = 0);
    void Consume(size_t len);
    void Produce(size_t len);
    void Shrink();
    void Swap(Buffer&);
    void Assurespace(size_t needsize);
    void Clear();

    static const size_t Kmaxbuffersize;
    static const size_t Kdefaultsize;
};

//自定义一个Buffer的容器，底层用list而不是vector实现，因为不需要坐标访问
//每个Buffer的大小都超过1k，防止出现多个小buffer导致list过长的情况，也避免频繁创建新的buffer
struct BufferVector
{
public:
//将typedef写在public和private是有区别的，和数据成员的区别一样
    typedef std::list<Buffer> Buffercontainer;
    typedef Buffercontainer::iterator iterator;
    typedef Buffercontainer::const_iterator const_iterator;

    static constexpr size_t kMinsize = 1024;
    Buffercontainer buffers;
    size_t totalbytes {0};
    
    BufferVector(){}
    BufferVector(Buffer&& buf)
    {
        Push(std::move(buf));
    }
//从buffers里添加或弹出新内容
    void Push(Buffer&& buf)
    {
        totalbytes += buf.readablesize();
        if(ShouldMerge())
        {
            buffers.back().PushData(buf.readaddr(), buf.readablesize());
            return;
        }
        buffers.push_back(std::move(buf));
    }
    void Push(const void* data, size_t len)
    {
        totalbytes += len;
        if(ShouldMerge())
        {
            buffers.back().PushData(data, len);
            return;
        }
        buffers.push_back(Buffer(data, len));
    }
    void Pop()
    {
        if(Isempty())
            return;
        totalbytes -= buffers.front().readablesize();
        buffers.pop_front();
    }

    bool Isempty() const
    {
        return buffers.empty();
    }
    size_t Totalbytes() const
    {
        return totalbytes;
    }
    void Clear()
    {
        buffers.clear();
        totalbytes = 0;
        return;
    }
    iterator begin()
    {
        return buffers.begin();
    }
    iterator end()
    {
        return buffers.end();
    }
    const_iterator begin() const
    {
        return buffers.begin();
    }
    const_iterator end() const
    {
        return buffers.end();
    }
    const_iterator cbegin()
    {
        return buffers.cbegin();
    }
    const_iterator cend()
    {
        return buffers.cend();
    }
private:
    bool ShouldMerge()
    {
        if(Isempty())
            return false;
        if(buffers.back().readablesize() < kMinsize)
            return true;
        return false;
    }
};

//字符串slice结构体
struct Slice
{ 
    const void* data;
    size_t len;
    Slice(const void* d = nullptr, size_t l = 0):data(d), len(l){}
    Slice& operator= (Slice&) = delete;
};

struct SliceVector
{
    typedef std::list<Slice> Slicecontainer;
    typedef Slicecontainer::iterator iterator;
    typedef Slicecontainer::const_iterator const_iterator;
    iterator begin()
    {
        return Slices.begin();
    }
    iterator end()
    {
        return Slices.end();
    }
    const_iterator begin() const
    {
        return Slices.begin();
    }
    const_iterator end() const
    {
        return Slices.end();
    }
    const_iterator cbegin()
    {
        return Slices.cbegin();
    }
    const_iterator cend()
    {
        return Slices.cend();
    }
    void Push(Slice&& slice_)
    {
        Slices.push_back(std::move(slice_));
    }
    void Push(const void* data, size_t len)
    {
        Push(Slice(data, len));
    }
    bool Isempty()
    {
        return Slices.empty();
    }
private:
    Slicecontainer Slices;
};

}

#endif


