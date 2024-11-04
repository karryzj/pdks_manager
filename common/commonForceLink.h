#ifndef COMMONFORCELINK_H
#define COMMONFORCELINK_H



#include "common.h"

/**
 *  @file Include this function to force linking of the db module
 */

namespace cm
{
CM_PUBLIC int _force_link_f ();
static int _force_link_target = _force_link_f ();
}



#endif // COMMONFORCELINK_H
