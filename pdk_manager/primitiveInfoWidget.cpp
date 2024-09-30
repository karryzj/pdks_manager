#include <QVBoxLayout>
#include "primitiveInfoWidget.h"

PrimitiveInfoWidget::PrimitiveInfoWidget(QWidget *parent)
    : QWidget{parent}
{
    setup_ui();
    fill_data();
}

void PrimitiveInfoWidget::setup_ui()
{
    QVBoxLayout *layout = new QVBoxLayout();

    // 创建缩略图标签
    mp_thumbnail_label = new QLabel(this);
    mp_thumbnail_label->setFixedSize(320, 240);
    mp_thumbnail_label->setAlignment(Qt::AlignCenter);  // 图片居中
    mp_thumbnail_label->setStyleSheet("border: 1px solid gray;");  // 给标签加一个灰色边框

    // 创建两个 QListWidget
    mp_coord_anchor_label = new QLabel(this);
    mp_coord_anchor_label->setFixedHeight(50);
    mp_coord_anchor_label->setStyleSheet("border: 1px solid black border-radius: 10px;");  // 给标签加一个黑色边框
    mp_params_listWidget = new QListWidget(this);
    mp_anchors_listWidget = new QListWidget(this);

    layout->addWidget(mp_thumbnail_label);
    layout->addWidget(mp_coord_anchor_label);
    layout->addWidget(mp_params_listWidget);
    layout->addWidget(mp_anchors_listWidget);
    setLayout(layout);
    setFixedWidth(352);
}

void PrimitiveInfoWidget::fill_data()
{
    // 设置缩略图 (使用图片路径)
    QPixmap thumbnail(":/img/top_align.png");
    mp_thumbnail_label->setPixmap(thumbnail.scaled(mp_thumbnail_label->size(), Qt::KeepAspectRatio));

    mp_coord_anchor_label->setText(tr("坐标锚点：(111,111)"));
    // 向第一个 listWidgetTop 添加数据
    mp_params_listWidget->addItem("Top Item 1");
    mp_params_listWidget->addItem("Top Item 2");
    mp_params_listWidget->addItem("Top Item 3");

    // 向第二个 listWidgetBottom 添加数据
    mp_anchors_listWidget->addItem("Bottom Item 1");
    mp_anchors_listWidget->addItem("Bottom Item 2");
    mp_anchors_listWidget->addItem("Bottom Item 3");
}
