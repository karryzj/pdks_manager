#include <QVBoxLayout>
#include <QLabel>
#include <QListWidgetItem>
#include "pdkParamTechRuleWidget.h"
#include "pdkParamTechRuleItemWidget.h"
#include "pdkDefines.h"
#include "paramDecl.h"
#include "pdkInstance.h"
#include "paramMgr.h"

namespace pdk
{
PdkParamTechRuleWidget::PdkParamTechRuleWidget(PdkInstance *pdk_instance, QWidget *parent)
    : QWidget(parent), mp_pdk_instance(pdk_instance), mp_list_widget(new QListWidget())
{
    setupUi();
    fillData();
}

void PdkParamTechRuleWidget::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout();

    // 创建表头（Header）
    QWidget *header = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(header);

    // 在表头添加标签和按钮
    QLabel *headerLabel = new QLabel(tr(PDK_PARAM_TECH_RULE_NAME), header);
    headerLayout->addWidget(headerLabel);
    header->setStyleSheet("QWidget {background-color: #1E1E1E; color: white;}");

    // 调整布局，右对齐按钮
    headerLayout->addStretch();  // 使用 stretch 将按钮右对齐
    layout->addWidget(header);
    layout->addWidget(mp_list_widget);
    setLayout(layout);
}

void PdkParamTechRuleWidget::fillData()
{
    mp_list_widget->clear();
    auto params = mp_pdk_instance->get_extra_param_mgr()->params();
    // 添加数据到模型
    foreach (auto param_decl, params)
    {
        QString old_expression = param_decl->expression();
        // 创建 QListWidgetItem
        QListWidgetItem *item = new QListWidgetItem(mp_list_widget);

        // 创建 CustomWidget 作为列表项的显示内容
        PdkParamTechRuleItemWidget *customWidget = new PdkParamTechRuleItemWidget(param_decl);
        connect(customWidget, &PdkParamTechRuleItemWidget::modify_param_item, [=](QString text)
        {
            pm::ParamDecl tmp_param(*param_decl);
            tmp_param.set_expression(old_expression);
            mp_pdk_instance->get_total_param_mgr()->update_param(param_decl);
            mp_pdk_instance->get_total_param_mgr()->refresh_params_value();
            auto is_update_ok = mp_pdk_instance->get_total_param_mgr()->isValid();
            if (is_update_ok)
            {
                mp_pdk_instance->get_extra_param_mgr()->update_param(param_decl);
                mp_pdk_instance->get_extra_param_mgr()->refresh_params_value();
            }
            else
            {
                mp_pdk_instance->get_extra_param_mgr()->update_param(param_decl);
                mp_pdk_instance->get_extra_param_mgr()->refresh_params_value();
                mp_pdk_instance->get_total_param_mgr()->update_param(&tmp_param);
                mp_pdk_instance->get_total_param_mgr()->refresh_params_value();
                customWidget->set_express(old_expression);
                customWidget->fill_data();
            }
        });
        // 将 CustomWidget 设置为 QListWidgetItem 的显示内容
        item->setSizeHint(customWidget->sizeHint());  // 调整大小以适应内容
        mp_list_widget->setItemWidget(item, customWidget);

    }

}

void PdkParamTechRuleWidget::clear_view()
{
    mp_list_widget->clear();
}


}
