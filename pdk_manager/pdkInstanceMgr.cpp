#include "pdkInstanceMgr.h"
#include "pdkInstance.h"

namespace pdk
{
PdkInstanceMgr* PdkInstanceMgr::mp_pdk_instance_mgr = nullptr;
PdkInstanceMgr::~PdkInstanceMgr()
{

}

PdkInstanceMgr *PdkInstanceMgr::instance()
{
    if (!mp_pdk_instance_mgr)
        mp_pdk_instance_mgr = new PdkInstanceMgr();
    return mp_pdk_instance_mgr;
}

PdkInstance *PdkInstanceMgr::new_pdk(const QString &name)
{
    PdkInstance *instance = new PdkInstance(name);
    m_pdks.insert(name, instance);
    return instance;
}

PdkInstance *PdkInstanceMgr::load_pdk(const QString &name)
{
    auto it = m_pdks.find(name);
    if (it == m_pdks.end())
        return NULL;

    PdkInstance *instance = it.value();
    if (instance->load())
        return NULL;
    return instance;
}

void PdkInstanceMgr::remove_pdk(const QString &name)
{
    m_pdks.remove(name);
}


PdkInstanceMgr::PdkInstanceMgr() {}

PdkInstanceMgr::PdkInstanceMgr(const PdkInstanceMgr &other)
{

}

}
