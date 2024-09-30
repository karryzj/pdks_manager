#ifndef SCOPETIMER_H
#define SCOPETIMER_H

#include <qstring.h>
#include <chrono>
#include "common.h"

namespace cm
{

class CM_PUBLIC ScopeTimer final
{
public:
    ScopeTimer(const QString& name);

    ~ScopeTimer();


private:
    QString  m_name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};

// 宏，用于在作用域开始时使用
#define SCOPE_TIMER(name) cm::ScopeTimer timer(name)

}

#endif // SCOPETIMER_H
