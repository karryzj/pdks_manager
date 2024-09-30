#include "layStyleFrame.h"
#include "layStyleDialog.h"
#include "layInfo.h"
#include <QDebug>
#include <QPainter>
#include <QWidget>
#include <layerUtils.h>

namespace ly
{
LayStyleFrame::LayStyleFrame(QWidget *parent, Qt::WindowFlags f) : QFrame(parent)
{
    setGeometry(50, 50, 200, 200);
    setFrameShape(QFrame::Box);
    setFrameShadow(QFrame::Plain);
    setLineWidth(2);
    setFrameStyle(QFrame::Box | QFrame::Plain);
}

LayStyleFrame::LayStyleFrame(LayerInfo style, QWidget *parent, Qt::WindowFlags f): QFrame(parent), m_style(style), m_old_style(style)
{
    setGeometry(50, 50, 200, 200);
    setFrameShape(QFrame::Box);
    setFrameShadow(QFrame::Plain);
    setLineWidth(2);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    update();
}

LayStyleFrame::~LayStyleFrame()
{

}

void LayStyleFrame::setStyle(const LayerInfo &style)
{
    m_style = style;
}

LayerInfo LayStyleFrame::style() const
{
    return m_style;
}

LayerInfo LayStyleFrame::getOldStyle() const
{
    return m_old_style;
}

void LayStyleFrame::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (m_style.layer_name() == "Location")
        return;

    LayStyleDialog dialog(m_style);
    if (dialog.exec() == QDialog::Accepted)
    {
        LayerInfo custom_style = dialog.getData();
        if (m_style != custom_style)
        {
            m_style = custom_style;
            update();
            emit dataChanged(custom_style);
        }
    }
}

void LayStyleFrame::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    layerUtils::setup_painter_by_layer_info(&m_style,painter);
    // 绘制矩形的边框
    painter.drawRect(rect());

}
}
