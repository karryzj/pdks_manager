#ifndef CRASHDUMP_H
#define CRASHDUMP_H

#include <Windows.h>
#include <Dbghelp.h>

#include "common.h"

namespace cm
{

class CM_PUBLIC CrashDump {
public:
    explicit CrashDump();
    ~CrashDump();
private:
    static LONG WINAPI UnhandledExceptionFilter(struct _EXCEPTION_POINTERS* pExceptionInfo);
private:
    LPTOP_LEVEL_EXCEPTION_FILTER        m_oldExceptionFilter;
};

}

#endif // CRASHDUMP_H
