#ifndef COMMON_DEFINES_H
#define COMMON_DEFINES_H

#include <cmath>  // HINT@leixunyong。为了std::round而增加这个头文件。

#define SAFE_DELETE(ptr) \
do { \
        if (ptr) { \
            delete ptr; \
            ptr = nullptr; \
    } \
} while (0)

#define TO_THREE_DECIMALS(value) (std::round((value) * 1000.0) / 1000.0)

#define CM_BOX_SCALE_MAX_VALUE                                        50000.0
#define CM_BOX_SCALE_MIN_VALUE                                        10e-3
#endif
