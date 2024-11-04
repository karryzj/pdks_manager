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

// 用于定义配置项的key
#define CM_LOGIC_GRID_RESOLUTION_KEY                                  "logic grid resolution"
#define CM_INNER_CORNER_RADIUS_KEY                                    "inner corner radius"
#define CM_INNER_CORNER_RADIUS_DEFAULT_VALUE                          1.0
#define CM_OUTER_CORNER_RADIUS_KEY                                    "outer corner radius"
#define CM_OUTER_CORNER_RADIUS_DEFAULT_VALUE                          1.0
#define CM_OUTER_CORNER_DIRECTION_KEY                                 "outer corner direction"
#define CM_OUTER_CORNER_DIRECTION_TOP_RIGHT                           "右上"
#define CM_OUTER_CORNER_DIRECTION_TOP_LEFT                            "左上"
#define CM_OUTER_CORNER_DIRECTION_BOTTOM_LEFT                         "左下"
#define CM_OUTER_CORNER_DIRECTION_BOTTOM_RIGHT                        "右下"
#define CM_OUTER_CORNER_DIRECTION_DEFAULT_VALUE                       CM_OUTER_CORNER_DIRECTION_TOP_RIGHT
#define CM_ARC_LEN_KEY                                                "arc len"
#define CM_ARC_LEN_DEFAULT_VALUE                                      1.0
#endif
