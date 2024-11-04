#include "styledialog.h"
#include <QDebug>
#include <QPainter>
#include "styleFrame.h"

namespace pdk
{
StyleFrame::StyleFrame(QWidget *parent, Qt::WindowFlags f) : QFrame(parent)
{
    setGeometry(50, 50, 200, 200);
    setFrameShape(QFrame::Box);
    setFrameShadow(QFrame::Plain);
    setLineWidth(2);
    setFrameStyle(QFrame::Box | QFrame::Plain);
}

StyleFrame::StyleFrame(BorderStyle style, QWidget *parent, Qt::WindowFlags f): QFrame(parent), m_style(style)
{
    setGeometry(50, 50, 200, 200);
    setFrameShape(QFrame::Box);
    setFrameShadow(QFrame::Plain);
    setLineWidth(2);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    update();
}

StyleFrame::~StyleFrame()
{

}

void StyleFrame::setStyle(const BorderStyle &style)
{
    m_style = style;
}

void StyleFrame::mouseDoubleClickEvent(QMouseEvent *event)
{
    StyleDialog dialog(m_style);
    if (dialog.exec() == QDialog::Accepted)
    {
        BorderStyle custom_style = dialog.getData();
        if (m_style != custom_style)
        {
            update();
            m_style = custom_style;
            emit dataChanged(custom_style);
        }
    }
}

void StyleFrame::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 设置虚线的画笔
    // 获取颜色的RGB值
    int r = (m_style.border_color >> 16) & 0xff;
    int g = (m_style.border_color >> 8) & 0xff;
    int b = (m_style.border_color) & 0xff;

    QPen pen(QColor(r, g, b));  // 设置颜色
    switch (m_style.border_line)
    {
    case 0:
        pen.setStyle(Qt::SolidLine);
        break;
    case 1:
        pen.setStyle(Qt::DashLine);
        break;
    case 2:
        pen.setStyle(Qt::DotLine);
        break;
    case 3:
        pen.setStyle(Qt::DashDotLine);
        break;
    case 4:
        pen.setStyle(Qt::DashDotDotLine);
        break;
    default:
        break;
    }
    painter.setPen(pen);

    int red = (m_style.fill_color >> 16) & 0xff;
    int green = (m_style.fill_color >> 8) & 0xff;
    int blue = (m_style.fill_color) & 0xff;

    QBrush brush;
    brush.setColor(QColor(red, green, blue));

    switch (m_style.fill_line)
    {
    case 0:

        break;
    case 1:
        brush.setStyle(Qt::HorPattern);
        break;
    case 2:
        // 绘制垂直线条
        brush.setStyle(Qt::VerPattern);
        break;
    case 3:
        brush.setStyle(Qt::CrossPattern);
        break;
    case 4:
        brush.setStyle(Qt::BDiagPattern);
        break;
    case 5:
        brush.setStyle(Qt::FDiagPattern);
        break;
    case 6:
        brush.setStyle(Qt::DiagCrossPattern);
        break;
    default:
        break;
    }
    painter.setBrush(brush);

    // 绘制矩形的边框
    painter.drawRect(rect());
}

BorderStyle StyleFrame::style() const
{
    return m_style;
}


}
