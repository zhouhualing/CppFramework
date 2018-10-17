/*
 *  Copyright (c) 2002 by Spirent Communications Inc.
 *  All Rights Reserved.
 *
 *  This software is confidential and proprietary to Spirent Communications Inc.
 *  No part of this software may be reproduced, transmitted, disclosed or used
 *  in violation of the Software License Agreement without the expressed
 *  written consent of Spirent Communications Inc.
 *
 */

#ifndef STC_TYPES_H
#define STC_TYPES_H

#ifndef WIN32
#include <memory>
#include <cstring>
#include <cstddef>
#include <cstdio>
#endif

#ifdef WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4251) // template element needs to have dll-interface
#endif

#if defined (__GNUC__)
// I'd normally include <config.h> but the yocto multi-lib config for 32bit is 
// setting SIZEOF_VOID_P is wrong for 32bit compiling and mismatches ACE configs.

#define GCC_VERSION (__GNUC__ * 10000                 \
                     + __GNUC_MINOR__ * 100           \
                     + __GNUC_PATCHLEVEL__)
#if GCC_VERSION > 40800
#define STDINT_HEADER_PRESENT
#endif
#endif

#ifndef STDINT_HEADER_PRESENT
// <stdint.h> is not available on this system

#if defined(__CYGWIN__) || defined (__sun__)
// cygwin and solaris (improperly) define this to a char in sys/types.h
typedef char               int8_t;
#else
// Linux defines this to a signed char in sys/types.h
typedef signed char        int8_t;
#endif

typedef unsigned char      uint8_t;

typedef short              int16_t;
typedef unsigned short     uint16_t;

#if defined (__GNUC__)
typedef int                int32_t;
typedef unsigned int       uint32_t;
#else
typedef int                int32_t;
typedef unsigned int       uint32_t;
#endif

#if defined (__GNUC__)
#if !defined (SUNOS_OS)
#if !defined (__x86_64__)
typedef long long          int64_t;
typedef unsigned long long uint64_t;
#else
typedef long int           int64_t;
typedef long unsigned int  uint64_t;
#endif
#define PRId64 "lld"
#define PRIu64 "llu"
#define PRIx64 "llx"
#define SCNd64 "lld"
#define SCNu64 "llu"
#define SCNx64 "llx"
#endif
#else
typedef __int64            int64_t;
typedef unsigned __int64   uint64_t;
#define PRId64 "I64d"
#define PRIu64 "I64u"
#define PRIx64 "I64x"
#define SCNd64 "I64d"
#define SCNu64 "I64u"
#define SCNx64 "I64x"
#endif

#else
#include <stdint.h>
#endif

/*
typedef struct
{
    uint64_t lo_64;
    uint32_t hi_32;
} uint96_t;
*/

#endif // STC_TYPES_H

