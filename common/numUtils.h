#ifndef NUMUTILS_H
#define NUMUTILS_H
#include "common.h"

#define PI 3.14159

namespace cm
{

class CM_PUBLIC NumUtils
{
public:
    static bool equal(double cm_from, int cm_to, int precision = 4);
    static bool is_zero(double val, int precision = 4);
};

}
#endif // NUMUTILS_H
