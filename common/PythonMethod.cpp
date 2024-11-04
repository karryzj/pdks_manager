#include "PythonMethod.h"
#include "lymMacro.h"
using namespace cm;

static PythonMethod* mp;
PythonMethod* PythonMethod::instance()
{
    if(mp){
        return mp;
    }else{
        mp = new PythonMethod();

        lym::Macro m;
        m.load_from("pymod/ququan/PythonMethodImpl.py");
        m.run();
    }
    return mp;
}

void PythonMethod::set_instance(PythonMethod* p)
{
    mp=p;
}

bool PythonMethod::export_pcell_to_gds(QString &pdk_name, QString &lib_name, QString &cell_name, QString &gds_file)
{
    if (cb_export_pcell_to_gds.can_issue ()) {
        return cb_export_pcell_to_gds.issue<cm::PythonMethod,bool,QString &,QString &,QString &,QString &> (&cm::PythonMethod::export_pcell_to_gds,pdk_name, lib_name, cell_name, gds_file);
    } else {
        //unset python method

        return false;
    }
}


