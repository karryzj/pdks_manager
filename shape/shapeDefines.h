#ifndef SHAPEDEFINES_H
#define SHAPEDEFINES_H

// 以下为图形的字符串的宏定义
#define SHAPE_POINT                                    "point"
#define SHAPE_CIRCLE                                   "circle"
#define SHAPE_CURVE                                    "curve"
#define SHAPE_ELLIPSE                                  "ellipse"
#define SHAPE_POLYGEN                                  "polygen"
#define SHAPE_QUADRANGLE                               "quadrangle"
#define SHAPE_RECTANGLE                                "rectangle"
#define SHAPE_SECTOR                                   "sector"
#define SHAPE_TRIANGLE                                 "triangle"
// 以下为图形的参数的字符串的宏定义
#define SHAPE_BASE_FIX_PARAM_COUNT                     1
#define SHAPE_ROTATE                                   "rotate"

#define SHAPE_CIRCLE_RADIUS                            "radius"

#define SHAPE_CURVE_EQUATION                           "equation"
#define SHAPE_CURVE_FIXED_POINT_SET                    "fixed point set"
#define SHAPE_CURVE_REVERSE                            "reverse"
#define SHAPE_CURVE_REVERSE_TURE                       "true"
#define SHAPE_CURVE_REVERSE_FALSE                      "false"

#define SHAPE_ELLIPSE_SEMI_MAJOR_AXIS                  "semi major axis"
#define SHAPE_ELLIPSE_SEMI_MINOR_AXIS                  "semi minor axis"
#define SHAPE_ELLIPSE_START_ANGLE                      "start angle"
#define SHAPE_ELLIPSE_END_ANGLE                        "end angle"

#define SHAPE_POLYGEN_IS_EXTRACT_PARAM                 "is extract param"

#define SHAPE_QUADRANGLE_WIDTH                         "width"
#define SHAPE_QUADRANGLE_HEIGHT                        "height"
#define SHAPE_QUADRANGLE_PARAMETER_1                   "parameter one"
#define SHAPE_QUADRANGLE_PARAMETER_2                   "parameter two"
#define SHAPE_QUADRANGLE_PARAMETER_3                   "parameter three"
#define SHAPE_QUADRANGLE_PARAMETER_4                   "parameter four"

#define SHAPE_RECTANGLE_WIDTH                          SHAPE_QUADRANGLE_WIDTH
#define SHAPE_RECTANGLE_HEIGHT                         SHAPE_QUADRANGLE_HEIGHT

#define SHAPE_SECTOR_INSIDE_RADIUS                     "inside radius"
#define SHAPE_SECTOR_OUTSIDE_RADIUS                    "outside radius"
#define SHAPE_SECTOR_START_ANGLE                       SHAPE_ELLIPSE_START_ANGLE
#define SHAPE_SECTOR_END_ANGLE                         SHAPE_ELLIPSE_END_ANGLE

#define SHAPE_TRIANGLE_WIDTH                           SHAPE_QUADRANGLE_WIDTH
#define SHAPE_TRIANGLE_HEIGHT                          SHAPE_QUADRANGLE_HEIGHT

// 以下为图形的参数的默认值的宏定义
#define SHAPE_ROTATE_DEFAULT_VALUE                     0.0
#define SHAPE_RECTANGLE_WIDTH_DEFAULT_VALUE            1.0
#define SHAPE_RECTANGLE_HEIGHT_DEFAULT_VALUE           1.0

#define SHAPE_CIRCLE_RADIUS_DEFAULT_VALUE              1.0

#define SHAPE_ELLIPSE_SEMI_MAJOR_AXIS_DEFAULT_VALUE    1.0
#define SHAPE_ELLIPSE_SEMI_MINOR_AXIS_DEFAULT_VALUE    0.5
#define SHAPE_ELLIPSE_START_ANGLE_DEFAULT_VALUE        0.0
#define SHAPE_ELLIPSE_END_ANGLE_DEFAULT_VALUE          360.0

#define SHAPE_QUADRANGLE_WIDTH_DEFAULT_VALUE           1.0
#define SHAPE_QUADRANGLE_HEIGHT_DEFAULT_VALUE          1.0
#define SHAPE_QUADRANGLE_ONE_DEFAULT_VALUE             0.0
#define SHAPE_QUADRANGLE_TWO_DEFAULT_VALUE             0.0
#define SHAPE_QUADRANGLE_THREE_DEFAULT_VALUE           0.0
#define SHAPE_QUADRANGLE_FOUR_DEFAULT_VALUE            0.0

#define SHAPE_SECTOR_INSIDE_RADIUS_DEFAULT_VALUE       0.5
#define SHAPE_SECTOR_OUTSIDE_RADIUS_DEFAULT_VALUE      1.0
#define SHAPE_SECTOR_START_ANGLE_DEFAULT_VALUE         0.0
#define SHAPE_SECTOR_END_ANGLE_DEFAULT_VALUE           90.0

#define SHAPE_TRIANGLE_WIDTH_DEFAULT_VALUE             1.0
#define SHAPE_TRIANGLE_HEIGHT_DEFAULT_VALUE            1.0

#define SHAPE_POINT_BOUNDING_RECT_DEFAULT_VALUE        20
#define SHAPE_POINT_HIGH_LIGHT_DEFAULT_VALUE           8

// 定义精度
#define SHAPE_COORD_PRECISION                          1e-3   // HINT@leixunyong。对应两个坐标，如果插值小于SHAPE_COORD_PRECISION则认为是对齐的。

// 定义原点的绘制属性
#define SHAPE_ORIGIN_POINT_RADIUS_RATIO                1.2

#endif // SHAPEDEFINES_H
