#ifndef LAYERUTILS_H
#define LAYERUTILS_H

#include "layerCommon.h"

class QPainter;
namespace ly
{
class LayerInfo;
class LY_PUBLIC layerUtils  // 用来放置一些通用的函数
{
public:
    layerUtils() = delete;

    static void setup_painter_by_layer_info(const LayerInfo* style, QPainter &painter);
    static void setup_painter_draw_attribute_layer_info(const LayerInfo* style, QPainter &painter);
    static void setup_painter_fill_attribute_layer_info(const LayerInfo* style, QPainter &painter);
};

}


#endif // LAYERUTILS_H
