#include <QHBoxLayout>
#include "drcMgrItemWidget.h"
namespace pdk
{
DrcMgrItemWidget::DrcMgrItemWidget(QWidget *parent)
    : QWidget(parent), mp_drc_lineEdit(new QLineEdit()), mp_modify_button(new QPushButton()), mp_del_button(new QPushButton())
{
    setup_ui();
    fill_data();
}

void DrcMgrItemWidget::setup_ui()
{
    QHBoxLayout *layout = new QHBoxLayout();

    mp_drc_lineEdit->setEnabled(false);
    mp_drc_lineEdit->setFixedSize(200, 30);
    // 使用样式表设置左边的图标
    mp_drc_lineEdit->setStyleSheet("QLineEdit { background-color: #2e2e2e; color: white;  }");

    mp_modify_button->setIcon(QIcon(":/img/edit.png"));
    mp_del_button->setIcon(QIcon(":/img/close.png"));

    layout->addWidget(mp_drc_lineEdit);
    layout->addWidget(mp_modify_button);
    layout->addWidget(mp_del_button);

    setLayout(layout);
}

void DrcMgrItemWidget::fill_data()
{

}

}
