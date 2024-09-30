#include <QColor>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include "layInfo.h"


namespace ly
{

QVector<unsigned int> LayerInfo::s_layer_color = {0xFF0014, 0x1955FF, 0x7DAEF7, 0x9B51EE, 0xC660F1,
            0xA6B65A, 0x7FB65A, 0xE8CE3B, 0xDEB22B, 0xB069E7
};

LayerInfo::LayerInfo() : m_visible(true)
{

}

LayerInfo::LayerInfo(const LayerInfo& other)
{
    m_layer_name = other.m_layer_name;
    m_border_style = other.m_border_style;
    m_border_color = other.m_border_color;
    m_fill_style = other.m_fill_style;
    m_fill_color = other.m_fill_color;
}

LayerInfo& LayerInfo::operator=(const LayerInfo& other)
{
    if (this == &other) return *this; // 防止自我赋值
    m_layer_name = other.m_layer_name;
    m_border_style = other.m_border_style;
    m_border_color = other.m_border_color;
    m_fill_style = other.m_fill_style;
    m_fill_color = other.m_fill_color;

    return *this;
}

LayerInfo::~LayerInfo()
{

}

LayerInfo::Border_Style LayerInfo::border_line() const
{
    return m_border_style;
}

void LayerInfo::set_border_line(Border_Style border_style)
{
    m_border_style = border_style;
}

QColor LayerInfo::border_color() const
{
    return m_border_color;
}

void LayerInfo::set_border_color(QColor border_color)
{
    m_border_color = border_color;
}

LayerInfo::Fill_Style LayerInfo::fill_line() const
{
    return m_fill_style;
}

void LayerInfo::set_fill_line(Fill_Style fill_style)
{
    m_fill_style = fill_style;
}

QColor LayerInfo::fill_color() const
{
    return m_fill_color;
}

void LayerInfo::set_fill_color(QColor fill_color)
{
    m_fill_color = fill_color;
}

void LayerInfo::getPainter(QPainter &painter, const LayerInfo &style) const
{
    // 设置虚线的画笔
    // 获取颜色的RGB值
    int r = style.border_color().red();
    int g = style.border_color().green();
    int b = style.border_color().blue();

    QPen pen(QColor(r, g, b));  // 设置颜色
    switch (style.border_line())
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

    QBrush brush;
    // 设置画笔样式
    // 获取颜色的RGB值
    int red = style.fill_color().red();
    int green = style.fill_color().green();
    int blue = style.fill_color().blue();

    brush.setColor(QColor(red, green, blue));

    switch (style.fill_line())
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

QString LayerInfo::layer_name() const
{
    return m_layer_name;
}

void LayerInfo::set_layer_name(const QString &layer_name)
{
    m_layer_name = layer_name;
}

bool LayerInfo::visible() const
{
    return m_visible;
}

void LayerInfo::set_visible(bool visible)
{
    m_visible = visible;
}

void LayerInfo::getPainter(QPainter &painter) const
{
    // 设置虚线的画笔
    // 获取颜色的RGB值
    int r = m_border_color.red();
    int g = m_border_color.green();
    int b = m_border_color.blue();

    QPen pen(QColor(r, g, b));  // 设置颜色
    switch (m_border_style)
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

    QBrush brush;
    // 设置画笔样式
    // 获取颜色的RGB值
    int red = m_fill_color.red();
    int green = m_fill_color.green();
    int blue = m_fill_color.blue();

    brush.setColor(QColor(red, green, blue));

    switch (m_fill_style)
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

}
