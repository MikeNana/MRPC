// The realization of logger
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <cstring>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <functional>

#include "TimeUtil.h"
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

Logger::~Logger()
{
    _CloseLogFile();
}

bool Logger::Init(unsigned int level, unsigned int dest, const char* dir)
{
    level_      = level;
    dest_       = dest;
    directory_  = dir ? dir : ".";

    if(directory_.back() == '/')
        directory_.pop_back();

    if(level_ == 0)
        return true;

    if(dest_ & logFile)
        return directory_ == "." || MakeDir(directory_.c_str());

    if(!(dest_ & logConsole))
    {
        std::cerr << "log has no output, but loglevel is " << level << std::endl;
        return false;
    }

    return true;
}

bool Logger::_CheckChangeFile()
{
    if(!file_.IsOpen())
        return true;
    
    return file_.Offset() + kMaxCharPerLog > kDefaultLogSize;
}

const std::string& Logger::_MakeFileName()
{
    char time[32];
    Time now;
    size_t len = now.FormatTime(time);
    time[len] = '\0';

    std::ostringstream pid;
    pid << "@" << ::getpid() << "-";

    seq_++;     // why not lock?
    fileName_ = directory_ + "/" + time + pid.str() + std::to_string(seq_) + ".log";

    return fileName_;
}

bool Logger::_OpenLogFile(const std::string& name)
{
    return file_.Open(name.data(), true);
}

void Logger::_CloseLogFile()
{
    return file_.Close();
}

//TODO config
static const size_t kFlushThreshold = 2 * 1024 * 1024;

void Logger::Flush(LogLevel level)
{
    assert(level == curlevel_);

    if(IsLevelForbid(curlevel_))
    {
        _Reset();
        return;
    }

    if(!(level & curlevel_) || (pos_ < kPrefixTimeLen + kPrefixLevelLen))
    {
        assert(false);
        return;
    }

    if(pos_ == kPrefixLevelLen + kPrefixTimeLen)    // empty log
        return;
    
    // refresh the time
    Time now;

    auto seconds = now.Millseconds() / 1000;
    if(seconds != lastLogSecond_)
    {
        now.FormatTime(tmpBuffer_);
        lastLogSecond_ = seconds;
    }
    else
    {
        auto msec = now.Microseconds() / 1000000;
        if(msec != lastLogMSecond_)
        {
            snprintf(tmpBuffer_ + 20, 7, "%06d", static_cast<int>(msec));
            tmpBuffer_[26] = ']';
            lastLogMSecond_ = msec;
        }
    }

    switch(level)
    {
        case logINFO:
            memcpy(tmpBuffer_ + kPrefixTimeLen, "[INF]:", kPrefixLevelLen);
            break;

        case logDEBUG:
            memcpy(tmpBuffer_ + kPrefixTimeLen, "[DBG]:", kPrefixLevelLen);
            break;
        
        case(logWARN):
            memcpy(tmpBuffer_ + kPrefixTimeLen, "[WRN]:", kPrefixLevelLen);
            break;

        case(logERROR):
            memcpy(tmpBuffer_ + kPrefixTimeLen, "[ERR]:", kPrefixLevelLen);
            break;

        case(logUSR):
            memcpy(tmpBuffer_ + kPrefixTimeLen, "[USR]:", kPrefixLevelLen);
            break;
    
        default:
            memcpy(tmpBuffer_ + kPrefixTimeLen, "[???]:", kPrefixLevelLen);
            break;
    }

    // initialize tid_, | thread_id ...
    if(tidLen_ == 0)
    {
        std::ostringstream oss;
        oss << std::this_thread::get_id();

        const auto& str = oss.str();
        tidLen_ = std::min<int>(str.size(), sizeof(tid_));  // avoid overflowing
        tid_[0] = '|';  // | thread id
        memcpy(tid_ + 1, str.data(), tidLen_);
        tidLen_ += 1;
    }

    // Put tid_ at tail, because tid_ length vary from different platform
    memcpy(tmpBuffer_ + pos_, tid_, tidLen_);
    pos_ += tidLen_;

    tmpBuffer_[pos_++] = '\n';
    tmpBuffer_[pos_]   = '\0';

    // get relevant bufferinfo
    BufferInfo* info = nullptr;
    {
        std::unique_lock<std::mutex> guard(mutex_);
        if(shutdown_)
        {
            std::cout << tmpBuffer_;
            return;
        }

        info = buffers_[std::this_thread::get_id()].get();
        if(!info)
            buffers_[std::this_thread::get_id()].reset(info = new BufferInfo());
        assert(!info->inuse_);
        info->inuse_ = true;
    }

    // Format : level info, length, log msg
    int logLevel = level;

    info->buffer_.PushData(&logLevel, sizeof(logLevel));
    info->buffer_.PushData(&pos_, sizeof(pos_));
    info->buffer_.PushData(tmpBuffer_, pos_);

    _Reset();

    // if current log is busy, handle it
    if(info->buffer_.readablesize() > kFlushThreshold)
    {
        info->inuse_ = false;
        LogManager::Instance().AddBusyLog(this);
    }
    else
    {
        info->inuse_ = false;
    }
}

void Logger::_Color(unsigned int color)
{
    const char* colorstring[Max] = 
    {
        "",
        "\033[1;31;40m",
        "\033[1;32;40m",
        "\033[1;33;40m",
        "\033[0m",
        "\033[1;34;40m",
        "\033[1;35;40m",
        "\033[1;37;40m",
    };

    fprintf(stdout, "%s", colorstring[color]);
}

Logger& Logger::operator<< (const char* msg)
{
    if(IsLevelForbid(curlevel_))
        return *this;

    const auto len = strlen(msg);
    if(pos_ + len >= kMaxCharPerLog)
        return *this;

    memcpy(tmpBuffer_ + pos_, msg, len);
    pos_ += len;

    return *this;
}

Logger& Logger::operator<< (const unsigned char* msg)
{
    return operator<< (reinterpret_cast<const char*> (msg));
}

Logger& Logger::operator<< (const std::string& msg)
{
    return operator<< (msg.c_str());
}

Logger& Logger::operator<< (void* ptr)
{
    
}


}   // end namespace mrpc





