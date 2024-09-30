#include <QVBoxLayout>
#include <QLabel>
#include <QListWidgetItem>
#include "pdkParamTechRuleWidget.h"
#include "pdkParamTechRuleItemWidget.h"
#include "pdkDefines.h"
#include "paramDecl.h"

namespace pdk
{
PdkParamTechRuleWidget::PdkParamTechRuleWidget(QWidget *parent)
    : QWidget(parent), mp_list_widget(new QListWidget())
{
    setupUi();
    fillData();
}

void PdkParamTechRuleWidget::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *label = new QLabel(tr(PDK_PARAM_TECH_RULE_NAME));

    // mp_listView->setModel(mp_model);

    // 设置自定义委托
    // PdkParamTechItemDelegate *delegate = new PdkParamTechItemDelegate(mp_listView);
    // mp_listView->setItemDelegate(delegate);
    layout->addWidget(label);
    layout->addWidget(mp_list_widget);
    setLayout(layout);
    // setFixedWidth(360);
}

void PdkParamTechRuleWidget::fillData()
{
    // 添加数据
    // for (int i = 0; i < 10; ++i)
    // {
    //     QStandardItem *item = new QStandardItem(QString("Item %1").arg(i + 1));
    //     item->setData(QString("Label Top %1").arg(i + 1), Qt::UserRole + 1);   // 设置顶部 label 文本
    //     item->setData(QString("Label Bottom %1").arg(i + 1), Qt::UserRole + 2); // 设置底部 label 文本
    //     mp_model->appendRow(item);
    // }

    // 添加数据到模型
    for (int i = 0; i < 10; ++i)
    {
        // 创建 QListWidgetItem
        QListWidgetItem *item = new QListWidgetItem(mp_list_widget);

        // 创建 CustomWidget 作为列表项的显示内容
        pm::ParamDecl *param_decl = new pm::ParamDecl("w", 20, "width");
        PdkParamTechRuleItemWidget *customWidget = new PdkParamTechRuleItemWidget(param_decl);

        // 将 CustomWidget 设置为 QListWidgetItem 的显示内容
        item->setSizeHint(customWidget->sizeHint());  // 调整大小以适应内容
        mp_list_widget->setItemWidget(item, customWidget);

    }

}


}
