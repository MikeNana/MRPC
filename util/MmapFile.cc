#include <cassert>
#include <string>
#include <fcntl.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <sys/mman.h>
#include <string>
#include <string.h>
#include "MmapFile.h"

namespace mrpc {
namespace internal {

static const size_t kDefaultSize = 1*1024*1024;
static const int kInvalidFile = -1;
static char* const kInvalidAddr = reinterpret_cast<char*>(-1);

OMmapFile::OMmapFile():file_(kInvalidFile),
                        memory_(kInvalidAddr),
                        offset_(0),
                        size_(0),
                        syncpos_(0)
{}

OMmapFile::~OMmapFile()
{
    Close();
}

void OMmapFile::_ExtendFileSize(size_t size)
{
    assert(file_ != kInvalidFile);

    if(size > size_)
        Truncate(size);
}

bool OMmapFile::Open(const std::string& file, bool bAppend)
{
    return Open(file.c_str(), bAppend);
}

bool OMmapFile::Open(const char* file, bool bAppend)
{
    Close();

    file_ = ::open(file, O_RDWR | O_CREAT | (bAppend ? O_APPEND : 0), 0644);
    // error occurred when opening file
    if(file_ == kInvalidFile)
    {
        char err[128];
        snprintf(err, sizeof(err) - 1, "OpenWriteOnly %s failed.\n", file);
        perror(err);
        assert(false);
        return false;
    }
    // elder file
    if(bAppend)
    {
        struct stat st;
        fstat(file_, &st);  // get the information of file_
        size_ = std::max<size_t>(kDefaultSize, st.st_size);
        offset_ = st.st_size;
    }
    // it is a new file
    else
    {
        size_ = kDefaultSize;
        offset_ = 0;
    }

    int ret = ::ftruncate(file_, size_);
    assert(ret == 0);
    
    return _MapWriteOnly();
}

void OMmapFile::Close()
{
    if(file_ != kInvalidFile)
    {   
        ::munmap(memory_, size_);       //unmap the relationship between memory and file
        ::ftruncate(file_, offset_);    //truncate size of file
        ::close(file_);

        file_ = kInvalidFile;
        memory_ = kInvalidAddr;
        size_ = 0;
        offset_ = 0;
        syncpos_ = 0;
    }
}

bool OMmapFile::Sync()
{
    if(file_ == kInvalidFile)
        return false;
    if(syncpos_ >= offset_)
        return false;
    
    ::msync(memory_ + syncpos_, offset_ - syncpos_, MS_SYNC);
    syncpos_ = offset_;
    return true;
}

bool OMmapFile::_MapWriteOnly()
{
    if(size_ == 0 || file_ == kInvalidFile)
    {
        assert(false);
        return false;
    }

    memory_ = (char*)::mmap(0, size_, PROT_WRITE, MAP_SHARED, file_, 0);
    return (memory_ != kInvalidAddr);
}

void OMmapFile::Truncate(std::size_t  size) {
    if (size == size_)
        return;

    size_ = size;
    int ret = ::ftruncate(file_, size_);
    assert (ret == 0);

    if (offset_> size_)
        offset_ = size_;

    _MapWriteOnly();
}

bool OMmapFile::IsOpen() const
{
    return file_ != kInvalidFile;
}

void OMmapFile::Write(const void* data, size_t len)
{
    _AssureSpace(len);

    assert(offset_ + len < size_);

    ::memcpy(memory_ + offset_, data, len);
    offset_ += len;
    assert(offset_ <= size_);
}

void OMmapFile::_AssureSpace(size_t len)
{
    size_t newsize = size_;
    while(offset_ + len > newsize)
    {
        if(newsize == 0)
            newsize = kDefaultSize;
        else
            newsize *= 2;
    }

    _ExtendFileSize(newsize);
}

}   // end namespace internal
}   // end namespace mrpcc