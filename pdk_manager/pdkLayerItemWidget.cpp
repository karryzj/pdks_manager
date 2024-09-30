#include <QHBoxLayout>
#include "pdkLayerItemWidget.h"
#include "pdkInstance.h"

namespace pdk
{
PdkLayerItemWidget::PdkLayerItemWidget(GdsLayerInfo *pri_pdk_info, QWidget *parent)
    : QWidget(parent), mp_name_lineEdit(new QLineEdit()), mp_layer_num_lineEdit(new QLineEdit()), mp_data_type_lineEdit(new QLineEdit()),
      mp_modify_button(new QPushButton()), mp_del_button(new QPushButton()), mp_pri_pdk_info(pri_pdk_info)
{

}

void PdkLayerItemWidget::setup_ui()
{
    QHBoxLayout *layout = new QHBoxLayout();

    mp_name_lineEdit->setEnabled(false);
    mp_name_lineEdit->setFixedSize(65, 30);

    mp_layer_num_lineEdit->setEnabled(false);
    mp_layer_num_lineEdit->setFixedSize(65, 30);

    mp_data_type_lineEdit->setEnabled(false);
    mp_data_type_lineEdit->setFixedSize(65, 30);

    mp_modify_button->setIcon(QIcon(":/img/edit.png"));
    mp_del_button->setIcon(QIcon(":/img/close.png"));

    layout->addWidget(mp_name_lineEdit);
    layout->addWidget(mp_layer_num_lineEdit);
    layout->addWidget(mp_data_type_lineEdit);
    layout->addWidget(mp_modify_button);
    layout->addWidget(mp_del_button);

    setLayout(layout);
    setFixedSize(350, 40);
}

}
