#include <QHBoxLayout>
#include "techRuleItemWidget.h"

namespace pdk
{

TechRuleItemWidget::TechRuleItemWidget(const QString &tech_rule, QWidget *parent)
    : QWidget(parent), mp_rule_lineEdit(new QLineEdit()), mp_modify_button(new QPushButton()),
      mp_del_button(new QPushButton()), m_tech_rule(tech_rule)
{
    setup_ui();
}

void TechRuleItemWidget::setup_ui()
{
    QHBoxLayout *layout = new QHBoxLayout();

    mp_rule_lineEdit->setEnabled(false);
    mp_rule_lineEdit->setFixedSize(200, 30);
    // 使用样式表设置左边的图标
    mp_rule_lineEdit->setStyleSheet("QLineEdit { padding-left: 30px; background: url(:img/tc_ty.png) no-repeat; "
                                    "background-color: #2e2e2e; color: white;  }");

    mp_modify_button->setIcon(QIcon(":/img/edit.png"));
    mp_del_button->setIcon(QIcon(":/img/close.png"));

    layout->addWidget(mp_rule_lineEdit);
    layout->addWidget(mp_modify_button);
    layout->addWidget(mp_del_button);

    setLayout(layout);
    setFixedSize(350, 40);
}

}
