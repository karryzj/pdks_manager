#include <QVBoxLayout>
#include "primitiveInfoWidget.h"
#include "pdkInstance.h"
#include "paramMgr.h"
#include "rule.h"

namespace pdk
{
PrimitiveInfoWidget::PrimitiveInfoWidget(PdkInstance *pdk_instance, QWidget *parent)
    : QWidget(parent), mp_pdk_instance(pdk_instance)
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
    mp_coord_anchor_label->setStyleSheet("QLabel {"
                                         "background-color: #1e1e1e;"
                                         "color: white;"  // 设置文本颜色为白色，以便在黑色背景上可见
                                         "border-radius: 15px;"  // 设置圆角半径
                                         "padding: 10px;"  // 设置内边距
                                         "}");  // 给标签加一个黑色边框
    QLabel *rule_label = new QLabel(tr("参数约束"));
    mp_params_listWidget = new QListWidget(this);
    QLabel *anchor_label = new QLabel(tr("所有锚点"));
    mp_anchors_listWidget = new QListWidget(this);

    layout->addWidget(mp_thumbnail_label);
    layout->addWidget(mp_coord_anchor_label);
    layout->addWidget(rule_label);
    layout->addWidget(mp_params_listWidget);
    layout->addWidget(anchor_label);
    layout->addWidget(mp_anchors_listWidget);
    setLayout(layout);
    setFixedWidth(352);
}

void PrimitiveInfoWidget::fill_data()
{
    // 设置缩略图 (使用图片路径)
    QString thumbnail_file = mp_pdk_instance->path() + "/" + mp_pdk_instance->pcell_thumbnail();
    QPixmap thumbnail(thumbnail_file);
    mp_thumbnail_label->setPixmap(thumbnail.scaled(mp_thumbnail_label->size(), Qt::KeepAspectRatio));

    QString coord_anchor = mp_pdk_instance->coord_anchor().pos.x().to_str() + "," + mp_pdk_instance->coord_anchor().pos.y().to_str();
    mp_coord_anchor_label->setText(tr("坐标锚点：(" + coord_anchor.toUtf8()+ ")"));

    auto rules = mp_pdk_instance->param_mgr()->rules();
    mp_params_listWidget->clear();
    foreach (auto rule, rules)
    {
        mp_params_listWidget->addItem(rule.to_str());
    }

    auto anchors = mp_pdk_instance->anchors();
    mp_anchors_listWidget->clear();
    foreach (auto anchor, anchors)
    {
        QString anchor_str = anchor.pos.x().to_str() + "," + anchor.pos.y().to_str();
        mp_anchors_listWidget->addItem(anchor_str);
    }

}

void PrimitiveInfoWidget::clear_view()
{
    mp_thumbnail_label->clear();
    mp_coord_anchor_label->clear();
    mp_params_listWidget->clear();
    mp_anchors_listWidget->clear();
}

}
