#include "scopeTimer.h"
#include "qdebug.h"

using namespace cm;

ScopeTimer::ScopeTimer(const QString &name)
{
    m_name = name;
    m_start = std::chrono::high_resolution_clock::now();
}

ScopeTimer::~ScopeTimer()
{
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - m_start;
    qDebug() << "Scope \"" << m_name << "\" executed in " << duration.count() << " ms" ;
}

