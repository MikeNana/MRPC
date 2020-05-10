#ifndef MMAPFILE_H_
#define MMAPFILE_H_

#include <string>

namespace mrpc {
namespace internal {

class OMmapFile
{

private:
    int file_;
    char* memory_;
    std::size_t syncpos_;
    std::size_t offset_;
    std::size_t size_;

    bool _MapWriteOnly();
    void _ExtendFileSize(std::size_t size);
    void _AssureSpace(std::size_t size);
public:
    OMmapFile();
    ~OMmapFile();

    bool Open(const std::string& file, bool bAppend = true);
    bool Open(const char* file, bool bAppend = true);
    void Close();
    bool Sync();

    void Truncate(std::size_t size);

    void Write(const void* data, std::size_t len);
    template<typename T>
    void Write(const T& t);

    std::size_t Offset() const
    {
        return offset_;
    }
    bool IsOpen() const;
};

template<typename T>
inline void OMmapFile::Write(const T& t)
{
    this->Write(&t, sizeof(t));
}


}   //end namespace internal
}   //end namespace mrpc
#endif