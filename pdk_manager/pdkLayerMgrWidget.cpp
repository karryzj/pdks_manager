#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "pdkLayerMgrWidget.h"
#include "pdkLayerItemWidget.h"
#include "pdkDefines.h"
#include "pdkInstance.h"
#include "gdsLayerInfo.h"

namespace pdk
{
PdkLayerMgrWidget::PdkLayerMgrWidget(PdkInstance *pdk_instance, QWidget *parent)
    : QWidget(parent), mp_pdk_instance(pdk_instance), mp_list_widget(new QListWidget())
{
    setup_ui();
    fill_data();
}

void PdkLayerMgrWidget::setup_ui()
{
    QVBoxLayout *layout = new QVBoxLayout();

    // 创建表头（Header）
    QWidget *header = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(header);

    // 在表头添加标签和按钮
    QLabel *headerLabel = new QLabel(tr(PDK_PRI_LAYER_LIST_NAME), header);
    headerLayout->addWidget(headerLabel);

    // 在表头右边添加按钮
    mp_add_button = new QPushButton();
    mp_add_button->setIcon(QIcon(":/img/add.png"));
    connect(mp_add_button, &QPushButton::clicked, this, &PdkLayerMgrWidget::add_gds_layer);
    header->setStyleSheet("QWidget {background-color: #1E1E1E; color: white;}");

    // 调整布局，右对齐按钮
    headerLayout->addStretch();  // 使用 stretch 将按钮右对齐
    headerLayout->addWidget(mp_add_button);
    layout->addWidget(header);

    layout->addWidget(mp_list_widget);
    setLayout(layout);
    // setFixedWidth(360);
}

void PdkLayerMgrWidget::fill_data()
{
    QString layer_json = mp_pdk_instance->pdk_path() + "/layer.json";
    mp_list_widget->clear();
    // 添加数据到模型
    auto gds_layers = mp_pdk_instance->pdk_gds_layers();
    foreach (const auto &gds_layer, gds_layers)
    {
        // 创建 QListWidgetItem
        QListWidgetItem *item = new QListWidgetItem(mp_list_widget);

        PdkLayerItemWidget *customWidget = new PdkLayerItemWidget(gds_layer);
        connect(customWidget, &PdkLayerItemWidget::delete_layer_item, [=](PdkLayerItemWidget *widget)
        {
            mp_pdk_instance->remove_pdk_gds_layer(gds_layer);
            mp_pdk_instance->save_layer_json_file(layer_json);
            int row = mp_list_widget->row(item);
            delete mp_list_widget->takeItem(row);
        });
        connect(customWidget, &PdkLayerItemWidget::modify_layer_item, [=](GdsLayerInfo *cur_gds_layer)
        {
            mp_pdk_instance->save_layer_json_file(layer_json);
        });

        // 将 CustomWidget 设置为 QListWidgetItem 的显示内容
        item->setSizeHint(customWidget->sizeHint());  // 调整大小以适应内容
        mp_list_widget->addItem(item);
        mp_list_widget->setItemWidget(item, customWidget);

    }

}

void PdkLayerMgrWidget::add_gds_layer()
{
    QString layer_json = mp_pdk_instance->pdk_path() + "/layer.json";
    auto pdk_gds_layers = mp_pdk_instance->pdk_gds_layers();
    // 创建 CustomWidget 作为列表项的显示内容
    GdsLayerInfo *info = new GdsLayerInfo();
    info->layer_name = "M1";
    info->layer_num = 0;
    info->data_type = 0;
    info->border_style.border_line = 0;
    info->border_style.border_color = get_color(mp_list_widget->count() % 10);
    info->border_style.fill_line = mp_list_widget->count() % 2 + 4;
    info->border_style.fill_color = get_color(mp_list_widget->count() % 10);

    auto it = std::find_if(pdk_gds_layers.begin(), pdk_gds_layers.end(), [info](auto layer)
    {
        return layer->layer_num == info->layer_num && layer->data_type == info->data_type;
    });
    if (it == pdk_gds_layers.end())
    {
        mp_pdk_instance->add_pdk_gds_layer(info);
        mp_pdk_instance->save_layer_json_file(layer_json);
        QListWidgetItem *item = new QListWidgetItem(mp_list_widget);
        PdkLayerItemWidget *customWidget = new PdkLayerItemWidget(info);
        connect(customWidget, &PdkLayerItemWidget::delete_layer_item, [=](PdkLayerItemWidget *widget)
        {
            mp_pdk_instance->remove_pdk_gds_layer(info);
            mp_pdk_instance->save_layer_json_file(layer_json);
            int row = mp_list_widget->row(item);
            delete mp_list_widget->takeItem(row);
        });
        connect(customWidget, &PdkLayerItemWidget::modify_layer_item, [=](GdsLayerInfo *cur_gds_layer)
        {
            mp_pdk_instance->save_layer_json_file(layer_json);
        });

        // 将 CustomWidget 设置为 QListWidgetItem 的显示内容
        item->setSizeHint(customWidget->sizeHint());  // 调整大小以适应内容
        mp_list_widget->addItem(item);
        mp_list_widget->setItemWidget(item, customWidget);
    }
}

void PdkLayerMgrWidget::del_current_item()
{

}

}
