#include <cmath>

#include "numUtils.h"

namespace cm
{

bool NumUtils::equal(double cm_from, int cm_to, int precision)
{
    const double multiplier = std::pow(10.0, precision);
    return std::round(cm_from * multiplier) / multiplier == cm_to;
}

bool NumUtils::is_zero(double val, int precision)
{
    return NumUtils::equal(val, 0, precision);
}

}
