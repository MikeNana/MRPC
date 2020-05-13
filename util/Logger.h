#ifndef LOGGER_H_
#define LOGGER_H_

#include <atomic>  
#include <string>
#include <mutex>
#include <condition_variable>
#include <map>  
#include <thread>
#include <memory>

#include "Buffer.h"
#include "MmapFile.h"
enum LogLevel
{
    logINFO  = 0x01 << 0,
    logDEBUG = 0x01 << 1,
    logWARN  = 0x01 << 2,
    logERROR = 0x01 << 3,
    logUSR   = 0x01 << 4,
    logALL   = 0xffffffff
};

enum LogDEST
{
    logConsole = 0x01 << 0,
    logFile    = 0x01 << 1,
    logSocket  = 0x01 << 2
};

namespace mrpc
{
class Logger
{
public:
    friend class LogManager;

    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    void operator= (const Logger&) = delete;
    Logger(Logger&&) = delete;
    void operator= (Logger&&) = delete;
// default level: debug     default destination: file 
    bool Init(unsigned int level = logDEBUG, unsigned int dest = logFile, const char* pDir = 0);

    void Flush(LogLevel level);
    bool IsLevelForbid(unsigned int level) const
    {
        return !(level & level_);
    }

// << operator overloadding
    Logger& operator<< (const char* );
    Logger& operator<< (const std::string& );
    Logger& operator<< (const unsigned char* );
    Logger& operator<< (void* );
    Logger& operator<< (unsigned char);
    Logger& operator<< (char );
    Logger& operator<< (unsigned short );
    Logger& operator<< (short );
    Logger& operator<< (unsigned int );
    Logger& operator<< (int );
    Logger& operator<< (unsigned long );
    Logger& operator<< (long );
    Logger& operator<< (unsigned long long );
    Logger& operator<< (long long );
    Logger& operator<< (double );

    Logger& SetCurLevel(unsigned int level);

    void shutdown();
    bool Update();

private:   
    static const size_t kMaxCharPerLog = 2048;
    
    // ensure thread-safe by thread_local variables
    static thread_local char tmpBuffer_[kMaxCharPerLog];
    static thread_local std::size_t pos_;
    static thread_local int64_t lastLogSecond_;
    static thread_local int64_t lastLogMSecond_;
    static thread_local unsigned int curlevel_;
    static thread_local char tid_[16];
    static thread_local int tidLen_;

    struct BufferInfo
    {
        std::atomic<bool> inuse_{false};
        Buffer buffer_;
    };
    std::mutex mutex_;
    std::map<std::thread::id, std::unique_ptr<BufferInfo>> buffers_;
    bool shutdown_;

    // const vars from init()
    LogLevel level_;
    std::string directory_;
    unsigned int dest_;
    std::string fileName_;

    internal::OMmapFile file_;

    std::size_t _Log(const char* data, std::size_t len);

    bool _CheckChangeFile();
    const std::string& _MakeFileName();
    bool _OpenLogFile(const std::string& name);
    void _CloseLogFile();
    void _WriteLog(unsigned int level, std::size_t nlen, const char* data);
    void _Color(unsigned int color);
    void _Reset();

    static unsigned int seq_;
};
// must be singleton
class LogManager
{   
public:
    static LogManager& Instance();
    
    void start();
    void stop();

    std::shared_ptr<Logger> CreateLog(unsigned int level,
                                      unsigned int dest, 
                                      const char* dir = nullptr);
    void AddBusyLog(Logger* );
    Logger* NullLog()
    {
        return &nullLog_;
    } 
private:
    LogManager();

    Logger nullLog_;

};
}



#endif