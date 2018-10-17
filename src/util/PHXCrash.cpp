/// @file
/// @brief Crash tracing utilities
///
///  Copyright (c) 2007 by Spirent Communications Inc.
///  All Rights Reserved.
///
///  This software is confidential and proprietary to Spirent Communications Inc.
///  No part of this software may be reproduced, transmitted, disclosed or used
///  in violation of the Software License Agreement without the expressed
///  written consent of Spirent Communications Inc.
///

#include "StdAfx.h"
#if defined(_LINUX)

#include <cxxabi.h>
#include <dlfcn.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <unwind.h>
#include <vector>

#include <sys/param.h>
#include <sys/signal.h>

#include "PHXCrash.h"

extern char **_dl_argv;

namespace PHXCrash
{

///////////////////////////////////////////////////////////////////////////////

struct BacktraceArrayArg
{
    BacktraceArrayArg(void **theArray, size_t theSize) 
        : array(theArray),
          size(theSize),
          count(-1)
    {
    }
    
    void **array;
    size_t size;
    int count;
};
    
struct BacktraceVectorArg
{
    BacktraceVectorArg(std::vector<void *>& theArray) 
        : array(theArray),
          first(true)
    {
    }
    
    std::vector<void *>& array;
    bool first;
};

static _Unwind_Reason_Code BacktraceArrayHelper(struct _Unwind_Context *ctx, BacktraceArrayArg *arg)
{
    // We are first called with address in the Backtrace function. Skip it.
    if (arg->count != -1)
        arg->array[arg->count] = (void *) _Unwind_GetIP(ctx);

    if (++arg->count == (int) arg->size)
        return _URC_END_OF_STACK;

    return _URC_NO_REASON;
}
    
static _Unwind_Reason_Code BacktraceVectorHelper(struct _Unwind_Context *ctx, BacktraceVectorArg *arg)
{
    // We are first called with address in the Backtrace function. Skip it.
    if (arg->first)
        arg->first = false;
    else
        arg->array.push_back((void *) _Unwind_GetIP(ctx));
    
    return _URC_NO_REASON;
}

size_t Backtrace(void **pointers, size_t size)
{
    BacktraceArrayArg arg(pointers, size);
    if (size)
        _Unwind_Backtrace((_Unwind_Trace_Fn) BacktraceArrayHelper, &arg);
    
    return (arg.count != -1) ? (size_t) arg.count : 0;
}
    
void Backtrace(std::vector<void *>& pointers)
{
    BacktraceVectorArg arg(pointers);
    _Unwind_Backtrace((_Unwind_Trace_Fn) BacktraceVectorHelper, &arg);
}

///////////////////////////////////////////////////////////////////////////////

char *Trace(void **pointers, size_t frame, char *buffer, size_t maxBuffer)
{
    size_t pos = snprintf(buffer, maxBuffer, "#%-2u [%p] ", frame, pointers[frame]);

    Dl_info dli;
    if (dladdr(pointers[frame], &dli) != 0)
    {
        if (dli.dli_sname && dli.dli_fname)
        {
            int status = 0;
            char *c_name = abi::__cxa_demangle(dli.dli_sname, 0, 0, &status);
            const char *sname = (status != 0) ? dli.dli_sname : c_name;
                
            const ptrdiff_t offset = (char *) pointers[frame] - (char *) dli.dli_saddr;
            if (offset)
                snprintf(buffer + pos, maxBuffer - pos, "%s+%#x in %s", sname, offset, dli.dli_fname);
            else
                snprintf(buffer + pos, maxBuffer - pos, "%s in %s", sname, dli.dli_fname);

            if (c_name)
                free(c_name);
        }
        else if (dli.dli_fname)
            snprintf(buffer + pos, maxBuffer - pos, "??? in %s", dli.dli_fname);
        else
            strncpy(buffer + pos, "??? in ???", maxBuffer - pos);
    }
    else
        strncpy(buffer + pos, "??? in ???", maxBuffer - pos);

    return buffer;
}
    
///////////////////////////////////////////////////////////////////////////////

const std::string Trace(const std::vector<void *>& pointers, size_t frame)
{
    void *addr = pointers.at(frame);
    std::ostringstream oss;

    oss << "#" << std::dec << std::setw(2) << std::left << frame << " [" << addr << "] ";

    Dl_info dli;
    if (dladdr(addr, &dli) != 0)
    {
        if (dli.dli_sname && dli.dli_fname)
        {
            int status = 0;
            char *sname = abi::__cxa_demangle(dli.dli_sname, 0, 0, &status);
            if (status == 0)
                oss << sname;
            else
                oss << dli.dli_sname;

            if (sname)
                free(sname);
            
            const ptrdiff_t offset = (char *) addr - (char *) dli.dli_saddr;
            if (offset)
                oss << "+" << std::hex << std::showbase << offset;

            oss << " in " << dli.dli_fname;
        }
        else if (dli.dli_fname)
            oss << "??? in " << dli.dli_fname;
        else
            oss << "??? in ???";
    }
    else
        oss << "??? in ???";

    return oss.str();
}

///////////////////////////////////////////////////////////////////////////////

void SignalDump(const char *dumpFileName, int sig, void **pointers, size_t depth)
{
    std::fstream dumpFile(dumpFileName, std::fstream::out);
    const size_t maxBuffer = 1024;
    char buffer[maxBuffer];

    const char *sigName = "UNKNOWN";
    switch (sig)
    {
      case SIGHUP:
          sigName = "SIGHUP";
          break;

      case SIGINT:
          sigName = "SIGINT";
          break;
          
      case SIGQUIT:
          sigName = "SIGQUIT";
          break;

      case SIGILL:
          sigName = "SIGILL";
          break;
          
      case SIGABRT:
          sigName = "SIGABRT";
          break;
          
      case SIGFPE:
          sigName = "SIGFPE";
          break;
          
      case SIGKILL:
          sigName = "SIGKILL";
          break;
          
      case SIGSEGV:
          sigName = "SIGSEGV";
          break;
          
      case SIGPIPE:
          sigName = "SIGPIPE";
          break;
          
      case SIGALRM:
          sigName = "SIGALRM";
          break;
          
      case SIGTERM:
          sigName = "SIGTERM";
          break;
          
      case SIGUSR1:
          sigName = "SIGUSR1";
          break;
          
      case SIGUSR2:
          sigName = "SIGUSR2";
          break;
          
      case SIGCHLD:
          sigName = "SIGCHLD";
          break;
          
      case SIGCONT:
          sigName = "SIGCONT";
          break;
          
      case SIGSTOP:
          sigName = "SIGSTOP";
          break;
          
      case SIGTSTP:
          sigName = "SIGTSTP";
          break;
          
      case SIGTTIN:
          sigName = "SIGTTIN";
          break;
          
      case SIGTTOU:
          sigName = "SIGTTOU";
          break;
    }
    
    dumpFile << "Caught signal " << sig << " (" << sigName << "), backtrace follows:" << std::endl;
    
    for (size_t frame = 0; frame < depth; ++frame)
        dumpFile << "    " << Trace(pointers, frame, buffer, maxBuffer) << std::endl;

    dumpFile.close();
}

///////////////////////////////////////////////////////////////////////////////
    
};  // namespace PHXCrash

#endif 
