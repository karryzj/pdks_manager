#include <QHBoxLayout>
#include <QVBoxLayout>
#include "pdkParamTechRuleItemWidget.h"
#include "paramDecl.h"

namespace pdk
{

PdkParamTechRuleItemWidget::PdkParamTechRuleItemWidget(pm::ParamDecl *param_decl, QWidget *parent)
    :QWidget(parent), mp_param_decl(param_decl), mp_value_lineEdit(new QLineEdit()), mp_name_label(new QLabel()), mp_desc_label(new QLabel())
{
    setup_ui();
    fill_data();
}

QString PdkParamTechRuleItemWidget::get_result()
{
    QString val = mp_value_lineEdit->text().remove(' ');
    return val;
}

void PdkParamTechRuleItemWidget::set_express(const QString &exp)
{
    mp_param_decl->set_expression(exp);
}

void PdkParamTechRuleItemWidget::setup_ui()
{
    QHBoxLayout *layout = new QHBoxLayout();

    // mp_value_lineEdit->setEnabled(false);
    mp_value_lineEdit->setFixedSize(130, 40);
    // 使用样式表设置左边的图标
    mp_value_lineEdit->setStyleSheet("QLineEdit { background-color: #1e1e1e; color: white;  }");
    connect(mp_value_lineEdit, &QLineEdit::editingFinished, this, &PdkParamTechRuleItemWidget::changed_value);

    QVBoxLayout *label_layout = new QVBoxLayout();
    mp_name_label->setFixedSize(150, 20);
    mp_desc_label->setFixedSize(150, 20);
    label_layout->addWidget(mp_name_label);
    label_layout->addWidget(mp_desc_label);

    layout->addWidget(mp_value_lineEdit);
    layout->addLayout(label_layout);

    setLayout(layout);
}

void PdkParamTechRuleItemWidget::fill_data()
{
    mp_value_lineEdit->setText(mp_param_decl->expression());
    mp_name_label->setText(mp_param_decl->key());
    mp_desc_label->setText(mp_param_decl->desc());
}

void PdkParamTechRuleItemWidget::changed_value()
{
    QString val = mp_value_lineEdit->text().remove(' ');
    mp_param_decl->set_expression(val);
    emit modify_param_item(val);
}

}
