#include <QLabel>
#include <QHBoxLayout>
#include "layListItemWidget.h"
#include "layStyleFrame.h"
#include "layInfo.h"
#include <QDebug>

namespace ly
{

LayListItemWidget::LayListItemWidget(const LayerInfo &style, const QString &text, QWidget *parent)
    : QWidget(parent)
{
    // 创建一个前置的QFrame
    mp_frame = new LayStyleFrame(style, this);
    mp_frame->setFrameShape(QFrame::Box);
    mp_frame->setFixedSize(80, 40); // 设置frame的大小

    // 创建一个后置的QLabel
    mp_layer_check = new QCheckBox(text, this);
    mp_layer_check->setChecked(true);

    // 创建水平布局
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(mp_frame);
    layout->addWidget(mp_layer_check);
    layout->setContentsMargins(5, 5, 5, 5); // 设置边距
    layout->setSpacing(10); // 设置控件之间的间距

    setLayout(layout);

    connect(mp_frame, &LayStyleFrame::dataChanged, this, &LayListItemWidget::onStyleFrameChanged);
    connect(mp_layer_check, &QCheckBox::stateChanged, this, &LayListItemWidget::onLayerStateChanged);
}

void LayListItemWidget::onStyleFrameChanged(const LayerInfo &style)
{
    if (mp_frame->getOldStyle() != style)
    {
        emit styleChanged(style);
    }
}

void LayListItemWidget::onLayerStateChanged(int state)
{
    // if (mp_frame->getOldStyle() != style)
    // {
    //     emit styleChanged(style);
    // }
    qDebug() << mp_layer_check->text() << "," << state;
    emit layerStateChanged(mp_layer_check->text(), state);
}

LayListItemWidget::~LayListItemWidget()
{
    delete mp_frame;
}

LayerInfo LayListItemWidget::get_current_layerinfo() const
{
    return mp_frame->style();
}

int LayListItemWidget::get_current_check_state() const
{
    return mp_layer_check->checkState();
}

}
