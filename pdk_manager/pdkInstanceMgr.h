#ifndef PDKINSTANCEMGR_H
#define PDKINSTANCEMGR_H

#include <QMap>

namespace pdk
{
class PdkInstance;

class PdkInstanceMgr
{
public:
    ~PdkInstanceMgr();
    static PdkInstanceMgr *instance();
    PdkInstance *new_pdk(const QString &name = "pdk1");
    PdkInstance *load_pdk(const QString &name = "pdk1");
    void remove_pdk(const QString &name = "pdk1");

private:
    PdkInstanceMgr();
    PdkInstanceMgr(const PdkInstanceMgr &other);
    static PdkInstanceMgr* mp_pdk_instance_mgr;

    QMap<QString, PdkInstance*> m_pdks;

};

}
#endif // PDKINSTANCEMGR_H
