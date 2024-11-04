#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QInputDialog>
#include "techRuleWidget.h"
#include "techRuleItemWidget.h"
#include "pdkDefines.h"
#include "pdkInstance.h"
#include "paramMgr.h"

namespace pdk
{
TechRuleWidget::TechRuleWidget(PdkInstance *pdk_instance, QWidget *parent)
    : QWidget(parent), mp_pdk_instance(pdk_instance), mp_list_widget(new QListWidget())
{
    setup_ui();
    fill_data();
}

void TechRuleWidget::setup_ui()
{
    QVBoxLayout *layout = new QVBoxLayout();

    // 创建表头（Header）
    QWidget *header = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(header);

    // 在表头添加标签和按钮
    QLabel *headerLabel = new QLabel(tr(PDK_TECH_RULE_NAME), header);
    headerLayout->addWidget(headerLabel);

    // 在表头右边添加按钮
    mp_add_button = new QPushButton();
    mp_add_button->setIcon(QIcon(":/img/add.png"));
    connect(mp_add_button, &QPushButton::clicked, this, &TechRuleWidget::add_rule_item);
    header->setStyleSheet("QWidget {background-color: #1E1E1E; color: white;}");

    // 调整布局，右对齐按钮
    headerLayout->addStretch();  // 使用 stretch 将按钮右对齐
    headerLayout->addWidget(mp_add_button);
    layout->addWidget(header);

    layout->addWidget(mp_list_widget);
    setLayout(layout);
    // setFixedWidth(360);
}

void TechRuleWidget::fill_data()
{
    mp_list_widget->clear();
    auto rules = mp_pdk_instance->get_extra_param_mgr()->rules();
    foreach (const auto rule, rules)
    {
        // 创建 QListWidgetItem
        QListWidgetItem *item = new QListWidgetItem(mp_list_widget);

        // 创建 CustomWidget 作为列表项的显示内容
        TechRuleItemWidget *customWidget = new TechRuleItemWidget(rule.to_str());
        connect(customWidget, &TechRuleItemWidget::delete_rule_item, [this, item](TechRuleItemWidget *widget)
        {
            int row = mp_list_widget->row(item);
            delete mp_list_widget->takeItem(row);
        });
        connect(customWidget, &TechRuleItemWidget::modify_rule_item, [this, customWidget](QString text)
        {
            QString old_rule = customWidget->get_input_result();
            QString new_rule = customWidget->get_result();
            if (mp_pdk_instance->get_total_param_mgr()->isValid(pm::Rule(new_rule)))
            {
                mp_pdk_instance->get_extra_param_mgr()->del_rule(old_rule);
                mp_pdk_instance->get_extra_param_mgr()->add_rule(pm::Rule(new_rule));
                mp_pdk_instance->get_total_param_mgr()->del_rule(old_rule);
                mp_pdk_instance->get_total_param_mgr()->add_rule(pm::Rule(new_rule));
            }
            else
            {
                customWidget->set_tech_rule(old_rule);
                customWidget->fill_data();
            }
        });
        // 将 CustomWidget 设置为 QListWidgetItem 的显示内容
        item->setSizeHint(customWidget->sizeHint());  // 调整大小以适应内容
        mp_list_widget->setItemWidget(item, customWidget);

    }

}

void TechRuleWidget::clear_view()
{
    mp_list_widget->clear();
}

void TechRuleWidget::add_rule_item()
{
    bool ok = false;
    QString rule_str = QInputDialog::getText(nullptr, tr("工艺约束"), tr("约束:"), QLineEdit::Normal, "", &ok);
    if (ok && !rule_str.remove(' ').isEmpty())
    {
        pm::Rule newRule(rule_str);
        if (mp_pdk_instance->get_total_param_mgr()->isValid(newRule))
        {
            // 创建 QListWidgetItem
            QListWidgetItem *item = new QListWidgetItem(mp_list_widget);
            mp_pdk_instance->get_extra_param_mgr()->add_rule(newRule);
            mp_pdk_instance->get_total_param_mgr()->add_rule(newRule);

            TechRuleItemWidget *customWidget = new TechRuleItemWidget(newRule.to_str());
            connect(customWidget, &TechRuleItemWidget::delete_rule_item, [this, item](TechRuleItemWidget *widget)
            {
                int row = mp_list_widget->row(item);
                mp_pdk_instance->get_extra_param_mgr()->del_rule(pm::Rule(item->text()));
                mp_pdk_instance->get_total_param_mgr()->del_rule(pm::Rule(item->text()));
                delete mp_list_widget->takeItem(row);
            });
            connect(customWidget, &TechRuleItemWidget::modify_rule_item, [this, customWidget](QString text)
            {
                QString old_rule = customWidget->get_input_result();
                QString new_rule = customWidget->get_result();
                if (mp_pdk_instance->get_total_param_mgr()->isValid(pm::Rule(new_rule)))
                {
                    mp_pdk_instance->get_extra_param_mgr()->del_rule(old_rule);
                    mp_pdk_instance->get_extra_param_mgr()->add_rule(pm::Rule(new_rule));
                    mp_pdk_instance->get_total_param_mgr()->del_rule(old_rule);
                    mp_pdk_instance->get_total_param_mgr()->add_rule(pm::Rule(new_rule));
                }
                else
                {
                    customWidget->set_tech_rule(old_rule);
                    customWidget->fill_data();
                }
            });

            // 将 CustomWidget 设置为 QListWidgetItem 的显示内容
            item->setSizeHint(customWidget->sizeHint());  // 调整大小以适应内容
            mp_list_widget->addItem(item);
            mp_list_widget->setItemWidget(item, customWidget);
        }
    }
}

}
