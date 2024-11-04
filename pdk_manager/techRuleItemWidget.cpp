#include <QHBoxLayout>
#include <QInputDialog>
#include "techRuleItemWidget.h"

namespace pdk
{

TechRuleItemWidget::TechRuleItemWidget(const QString &tech_rule, QWidget *parent)
    : QWidget(parent), mp_rule_lineEdit(new QLineEdit()), mp_modify_button(new QPushButton()),
      mp_del_button(new QPushButton()), m_tech_rule(tech_rule), m_old_rule(tech_rule)
{
    setup_ui();
    fill_data();
}

QString TechRuleItemWidget::get_result()
{
    return m_tech_rule;
}

QString TechRuleItemWidget::get_input_result()
{
    return m_old_rule;
}

void TechRuleItemWidget::setup_ui()
{
    QHBoxLayout *layout = new QHBoxLayout();

    mp_rule_lineEdit->setEnabled(false);
    mp_rule_lineEdit->setFixedSize(200, 30);
    // 使用样式表设置左边的图标
    mp_rule_lineEdit->setStyleSheet("QLineEdit { padding-left: 30px; background: url(:img/tc_ty.png) no-repeat; "
                                    "background-color: #1e1e1e; color: white;  }");

    mp_modify_button->setIcon(QIcon(":/img/edit.png"));
    mp_del_button->setIcon(QIcon(":/img/close.png"));
    connect(mp_modify_button, &QPushButton::clicked, this, &TechRuleItemWidget::modify_rule_info);
    connect(mp_del_button, &QPushButton::clicked, this, &TechRuleItemWidget::del_rule_info);
    layout->addWidget(mp_rule_lineEdit);
    layout->addWidget(mp_modify_button);
    layout->addWidget(mp_del_button);

    setLayout(layout);
    setFixedSize(350, 40);
}

void TechRuleItemWidget::fill_data()
{
    mp_rule_lineEdit->setText(m_tech_rule);
}

QString TechRuleItemWidget::tech_rule() const
{
    return m_tech_rule;
}

void TechRuleItemWidget::set_tech_rule(const QString &tech_rule)
{
    m_tech_rule = tech_rule;
}

void TechRuleItemWidget::modify_rule_info()
{
    bool ok = false;
    m_old_rule = m_tech_rule;
    QString text = QInputDialog::getText(nullptr, tr("工艺约束"), tr("约束:"), QLineEdit::Normal, m_tech_rule, &ok);
    if (ok && !text.remove(' ').isEmpty() && m_tech_rule != text)
    {
        mp_rule_lineEdit->setText(text);
        m_tech_rule = text;
        emit modify_rule_item(text);
    }
}

void TechRuleItemWidget::del_rule_info()
{
    emit delete_rule_item(this);
}

}
