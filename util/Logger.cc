// The realization of logger
#include <sys/stat.h> 
#include <sys/types.h>
#include "Logger.h"


namespace mrpc
{
namespace 
{

enum LogColor
{
    Red = 1,
    Green,
    Yellow,
    Normal,
    Blue,
    Purple,
    White,
    Max,
};

}   // end namespace

// TODO config
static const size_t kDefaultLogSize = 32 * 1024 * 1024;

static const size_t kPrefixLevelLen = 6;
static const size_t kPrefixTimeLen = 27;

static bool MakeDir(const char* dir)
{
    if(mkdir(dir, 0755) != 0)
    {
        if(EEXIST != errno)
        {
            perror("MakeDir error");
            return false;
        }
    }
    return true;
}

thread_local char Logger::tmpBuffer_[Logger::kMaxCharPerLog];
thread_local std::size_t Logger::pos_ = kPrefixLevelLen + kPrefixTimeLen;
thread_local int64_t Logger::lastLogMSecond_ = -1;
thread_local int64_t Logger::lastLogSecond_ = -1;
thread_local unsigned int Logger::curlevel_ = 0;
thread_local char Logger::tid_[16] = "";
thread_local int Logger::tidLen_ = 0;

unsigned int Logger::seq_ = 0;

Logger::Logger() : 
            shutdown_(false),
            level_(logINFO),
            dest_(0)
{
    _Reset();
}

}   // end namespace mrpc





