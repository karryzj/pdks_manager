
#ifndef HDR_pmForceLink
#define HDR_pmForceLink

#include "pmCommon.h"

/**
 *  @file Include this function to force linking of the db module
 */

namespace pm
{
  PM_PUBLIC int _force_link_f ();
  static int _force_link_target = _force_link_f ();
}

#endif
