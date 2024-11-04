#include <QVBoxLayout>
#include <QLabel>
#include "layerSelectWidget.h"
#include "layerSelectItemWidget.h"
#include "pdkDefines.h"
#include "pdkInstance.h"
#include "gdsLayerInfo.h"

namespace pdk
{
LayerSelectWidget::LayerSelectWidget(PdkInstance *pdk_instance, QWidget *parent)
    : QWidget(parent), mp_pdk_instance(pdk_instance), mp_list_widget(new QListWidget())
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
    QLabel *pdk_layer_label = new QLabel(tr(PDK_GDS_LAYER_NAME), header);
    header->setStyleSheet("QWidget {background-color: #1E1E1E; color: white;}");

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
    mp_list_widget->clear();
    // 添加数据
    for (int i = 0; i < m_layer_list.size(); ++i)
    {
        // 创建 QListWidgetItem
        QListWidgetItem *item = new QListWidgetItem(mp_list_widget);
        // 设置 flags 允许 item 可编辑
        // item->setFlags(item->flags() | Qt::ItemIsEditable);
        QString gds_layer = "";
        auto gds_maps = mp_pdk_instance->gds_layer_info();
        if (gds_maps.contains(m_layer_list.at(i)))
        {
            auto gds_info = gds_maps.value(m_layer_list.at(i));
            gds_layer = gds_info->layer_name;
        }
        // 创建 CustomWidget 作为列表项的显示内容
        LayerSelectItemWidget *customWidget = new LayerSelectItemWidget(mp_pdk_instance, m_layer_list.at(i), gds_layer);

        // 将 CustomWidget 设置为 QListWidgetItem 的显示内容
        item->setSizeHint(customWidget->sizeHint());  // 调整大小以适应内容
        mp_list_widget->setItemWidget(item, customWidget);
    }
    // update();
}

void LayerSelectWidget::clear_view()
{
    mp_list_widget->clear();
}

QStringList LayerSelectWidget::gds_list() const
{
    return m_gds_list;
}

void LayerSelectWidget::set_gds_list(const QStringList &gds_list)
{
    m_gds_list = gds_list;
}

QStringList LayerSelectWidget::layer_list() const
{
    return m_layer_list;
}

void LayerSelectWidget::set_layer_list(const QStringList &layer_list)
{
    m_layer_list = layer_list;
}

}
