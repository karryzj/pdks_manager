#include "PythonMethod.h"
#include "gsiDecl.h"



namespace gsi
{

//python重载的类和方法
static Class<cm::PythonMethod> decl_PythonMethod ("common", "PythonMethod",
        gsi::method ("Instance", &cm::PythonMethod::instance,
                  "This method was added in version 0.29.\n"
                  )+
        gsi::method ("set_instance", &cm::PythonMethod::set_instance, gsi::arg ("ins")) +
        gsi::callback ("export_pcell_to_gds", &cm::PythonMethod::export_pcell_to_gds,&cm::PythonMethod::cb_export_pcell_to_gds, gsi::arg ("pdk"), gsi::arg ("lib"),gsi::arg("cell"),gsi::arg("gds"),
                      "This method was added in version 0.29.\n"
                      ) ,
      "@brief This namespace provides a collection of utility functions\n"
      "\n"
      "This class has been introduced in version 0.29."
      );

}
