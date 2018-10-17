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

// borrowed from Dave's PHXCrash from IL
// TODO: remove this once it's in a shared location or till we've rework the spec file.
#if defined(_LINUX)

#ifndef _PHX_CRASH_H_
#define _PHX_CRASH_H_

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

namespace PHXCrash
{
    /// @brief Generate a stack backtrace
    /// @note Avoids dynamic memory allocation, more suitable for use in crash signal handlers
    size_t Backtrace(void **pointers, size_t size);
    
    /// @brief Generate a stack backtrace
    void Backtrace(std::vector<void *>& pointers);

    /// @brief Generate a string-formatted version of a given stack frame
    /// @note Avoids dynamic memory allocation, more suitable for use in crash signal handlers
    char *Trace(void **pointers, size_t frame, char *buffer, size_t maxBuffer);
    
    /// @brief Generate a string-formatted version of a given stack frame
    const std::string Trace(const std::vector<void *>& pointers, size_t frame);

    /// @brief Write a signal dump file
    /// @note Avoids dynamic memory allocation
    void SignalDump(const char *dumpFileName, int sig, void **pointers, size_t depth);
};

///////////////////////////////////////////////////////////////////////////////
    
#endif

#endif

