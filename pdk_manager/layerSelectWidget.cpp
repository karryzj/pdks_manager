#include <QVBoxLayout>
#include <QLabel>
#include "layerSelectWidget.h"
#include "layerSelectItemWidget.h"
#include "pdkDefines.h"

namespace pdk
{
LayerSelectWidget::LayerSelectWidget(const QStringList &layer_list, const QStringList &gds_list, QWidget *parent)
    : QWidget(parent), mp_list_widget(new QListWidget),
      m_layer_list(layer_list), m_gds_list(gds_list)
{
    SetupUi();
    FillData();
}

void LayerSelectWidget::SetupUi()
{
    QVBoxLayout *layout = new QVBoxLayout();

    // 创建表头（Header）
    QWidget *header = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(header);

    // 在表头添加标签和按钮
    QLabel *pri_select_label = new QLabel(tr(PDK_PRI_LAYER_SELECT_NAME), header);
    QLabel *pri_layer_label = new QLabel(tr(PDK_PRI_LAYER_NAME), header);
    QLabel *pdk_layer_label = new QLabel(tr(PDK_PRI_LAYER_NAME), header);

    headerLayout->addWidget(pri_layer_label);
    headerLayout->addStretch();
    headerLayout->addWidget(pdk_layer_label);
    headerLayout->addStretch();

    layout->addWidget(pri_select_label);
    layout->addWidget(header);
    layout->addWidget(mp_list_widget);
    setFixedWidth(550);
    setLayout(layout);
}

void LayerSelectWidget::FillData()
{
    // 添加数据
    for (int i = 0; i < m_layer_list.size(); ++i)
    {
        // 创建 QListWidgetItem
        QListWidgetItem *item = new QListWidgetItem(mp_list_widget);
        // 设置 flags 允许 item 可编辑
        // item->setFlags(item->flags() | Qt::ItemIsEditable);

        // 创建 CustomWidget 作为列表项的显示内容
        LayerSelectItemWidget *customWidget = new LayerSelectItemWidget(m_layer_list.at(i), m_gds_list);

        // 将 CustomWidget 设置为 QListWidgetItem 的显示内容
        item->setSizeHint(customWidget->sizeHint());  // 调整大小以适应内容
        mp_list_widget->setItemWidget(item, customWidget);
    }
}

}
