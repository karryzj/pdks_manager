#ifndef PYTHONMETHOD_H
#define PYTHONMETHOD_H

#include "common.h"
#include "gsiCallback.h"
#include "gsiObject.h"
namespace cm
{

class CM_PUBLIC PythonMethod
{
public:
    static PythonMethod *instance();
    void set_instance(PythonMethod* p);

    virtual bool export_pcell_to_gds(QString &pdk_name, QString &lib_name, QString &cell_name, QString &gds_file);
    gsi::Callback cb_export_pcell_to_gds;
private:


};

}


#endif // PYTHONMETHOD_H
