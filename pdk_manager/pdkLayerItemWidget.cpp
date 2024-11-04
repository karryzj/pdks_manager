#include <QHBoxLayout>
#include "pdkLayerItemWidget.h"
#include "pdkInstance.h"
#include "pdkGdsLayerDialog.h"
#include "gdsLayerInfo.h"
#include "styleFrame.h"

namespace pdk
{
PdkLayerItemWidget::PdkLayerItemWidget(GdsLayerInfo *pri_pdk_info, QWidget *parent)
    : QWidget(parent), mp_style_frame(new StyleFrame()), mp_name_lineEdit(new QLineEdit()),
      mp_layer_num_lineEdit(new QLineEdit()), mp_data_type_lineEdit(new QLineEdit()),
      mp_modify_button(new QPushButton()), mp_del_button(new QPushButton()), mp_pri_pdk_info(pri_pdk_info)
{
    setup_ui();
    fill_data();
}

PdkLayerItemWidget::~PdkLayerItemWidget()
{
    delete mp_style_frame;
    delete mp_name_lineEdit;
    delete mp_layer_num_lineEdit;
    delete mp_data_type_lineEdit;
    delete mp_modify_button;
    delete mp_del_button;
}

GdsLayerInfo *PdkLayerItemWidget::get_result()
{
    return mp_pri_pdk_info;
}

void PdkLayerItemWidget::setup_ui()
{
    QHBoxLayout *layout = new QHBoxLayout();

    mp_style_frame->setStyle(mp_pri_pdk_info->border_style);
    mp_style_frame->setFrameShape(QFrame::Box);
    mp_style_frame->setFixedSize(80, 40); // 设置frame的大小
    mp_style_frame->setEnabled(false);

    setStyleSheet("QLineEdit {"
                  "background-color: #1E1E1E;"
                  "color: white;"  // 设置文本颜色为白色，以便在深色背景上可见
                  "border: 1px solid #555555;"  // 设置边框颜色
                  "border-radius: 5px;"  // 设置圆角半径
                  "padding: 5px;"  // 设置内边距
                  "}");
    mp_name_lineEdit->setEnabled(false);
    mp_name_lineEdit->setFixedSize(65, 30);

    mp_layer_num_lineEdit->setEnabled(false);
    mp_layer_num_lineEdit->setFixedSize(65, 30);

    mp_data_type_lineEdit->setEnabled(false);
    mp_data_type_lineEdit->setFixedSize(65, 30);

    mp_modify_button->setIcon(QIcon(":/img/edit.png"));
    mp_del_button->setIcon(QIcon(":/img/close.png"));

    connect(mp_modify_button, &QPushButton::clicked, this, &PdkLayerItemWidget::modify_gds_info);
    connect(mp_del_button, &QPushButton::clicked, this, &PdkLayerItemWidget::del_gds_info);

    layout->addWidget(mp_style_frame);
    layout->addWidget(mp_name_lineEdit);
    layout->addWidget(mp_layer_num_lineEdit);
    layout->addWidget(mp_data_type_lineEdit);
    layout->addWidget(mp_modify_button);
    layout->addWidget(mp_del_button);

    setLayout(layout);
    setFixedSize(450, 40);
}

void PdkLayerItemWidget::fill_data()
{
    mp_name_lineEdit->setText(mp_pri_pdk_info->layer_name);
    mp_layer_num_lineEdit->setText(QString::number(mp_pri_pdk_info->layer_num));
    mp_data_type_lineEdit->setText(QString::number(mp_pri_pdk_info->data_type));
    mp_style_frame->setStyle(mp_pri_pdk_info->border_style);
}

void PdkLayerItemWidget::modify_gds_info()
{
    PdkGdsLayerDialog dialog(mp_pri_pdk_info);
    // PdkLayerItemGdsInfoDialog dialog(mp_pri_pdk_info);
    if (dialog.exec() == QDialog::Accepted)
    {
        fill_data();
        emit modify_layer_item(mp_pri_pdk_info);
    }

}

void PdkLayerItemWidget::del_gds_info()
{
    emit delete_layer_item(this);
}

}
