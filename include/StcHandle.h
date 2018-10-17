#ifndef INCLUDED_STC_HANDLE_H
#define INCLUDED_STC_HANDLE_H
#include "StcTypes.h"

typedef uint32_t StcHandle;
const StcHandle NULL_STCHANDLE = 0;
const StcHandle STC_SYSTEM_HANDLE = 1;
const StcHandle STC_PROJECT_HANDLE = 2;

// TODO: make this a weak pointer class instead

#endif // INCLUDED_STC_HANDLE_H

