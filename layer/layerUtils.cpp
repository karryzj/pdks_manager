#include "layerUtils.h"

#include "layInfo.h"

#include <qpainter.h>

using namespace ly;

void layerUtils::setup_painter_by_layer_info(const LayerInfo* style, QPainter &painter)
{
    if(nullptr == style)
    {
        return;
    }
    setup_painter_draw_attribute_layer_info(style,painter);
    setup_painter_fill_attribute_layer_info(style,painter);
}

void layerUtils::setup_painter_draw_attribute_layer_info(const LayerInfo* style, QPainter &painter)
{
    if(nullptr == style)
    {
        return;
    }
    // 设置虚线的画笔
    // 获取颜色的RGB值
    int r = style->border_color().red();
    int g = style->border_color().green();
    int b = style->border_color().blue();

    QPen pen(QColor(r, g, b));  // 设置颜色
    switch (style->border_line())
    {
    case LayerInfo::SolidLine:
        pen.setStyle(Qt::SolidLine);
        break;
    case LayerInfo::DashLine:
        pen.setStyle(Qt::DashLine);
        break;
    case LayerInfo::DotLine:
        pen.setStyle(Qt::DotLine);
        break;
    case LayerInfo::DashDotLine:
        pen.setStyle(Qt::DashDotLine);
        break;
    case LayerInfo::DashDotDotLine:
        pen.setStyle(Qt::DashDotDotLine);
        break;
    default:
        break;
    }
    painter.setPen(pen);
}

void layerUtils::setup_painter_fill_attribute_layer_info(const LayerInfo* style, QPainter &painter)
{
    if(nullptr == style)
    {
        return;
    }
    QBrush brush;
    // 设置画笔样式
    // 获取颜色的RGB值
    int red = style->fill_color().red();
    int green = style->fill_color().green();
    int blue = style->fill_color().blue();

    brush.setColor(QColor(red, green, blue));

    switch (style->fill_line())
    {
    case LayerInfo::NotFill:

        break;
    case LayerInfo::HorizontalLine:
        // 绘制水平线条
        brush.setStyle(Qt::HorPattern);
        break;
    case LayerInfo::VerticalLine:
        // 绘制垂直线条
        brush.setStyle(Qt::VerPattern);
        break;
    case LayerInfo::GridLine:
        brush.setStyle(Qt::CrossPattern);
        break;
    case LayerInfo::LeftDiagonal:
        brush.setStyle(Qt::BDiagPattern);
        break;
    case LayerInfo::RightDiagonal:
        brush.setStyle(Qt::FDiagPattern);
        break;
    case LayerInfo::GridDiagonal:
        brush.setStyle(Qt::DiagCrossPattern);
        break;
    case LayerInfo::FullFill:

        break;
    default:
        break;
    }
    painter.setBrush(brush);
}


