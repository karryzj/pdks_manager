#include <QHBoxLayout>
#include <QLabel>
#include "layerSelectItemWidget.h"

namespace pdk
{
LayerSelectItemWidget::LayerSelectItemWidget(const QString &layer, const QStringList &gds_list, QWidget *parent)
    : QWidget(parent), m_layer_name(layer), m_gds_list(gds_list),
      mp_layer_lineEdit(new QLineEdit()), mp_gds_info_comboBox(new QComboBox())
{
    setupUi();
    fillData();
}

LayerSelectItemWidget::~LayerSelectItemWidget()
{
    delete mp_layer_lineEdit;
    delete mp_gds_info_comboBox;
}

void LayerSelectItemWidget::setupUi()
{
    QHBoxLayout *layout = new QHBoxLayout();

    // mp_layer_lineEdit->setEnabled(false);
    mp_layer_lineEdit->setFixedSize(200, 30);
    // 使用样式表设置左边的图标
    mp_layer_lineEdit->setStyleSheet("QLineEdit { padding-left: 30px; background: url(:img/tc_ty.png) no-repeat; "
                                     "background-color: #2e2e2e; color: white;  }");

    QLabel *select_label = new QLabel();
    QPixmap pixmap(":/img/ty_pdk.png");  // 使用资源中的图片
    select_label->setPixmap(pixmap);  // 设置图标
    select_label->setAlignment(Qt::AlignCenter);  // 居中对齐
    select_label->setFixedSize(30, 20);
    mp_gds_info_comboBox->setFixedSize(200, 30);

    layout->setStretchFactor(mp_layer_lineEdit, 1);
    layout->setStretchFactor(mp_gds_info_comboBox, 1);

    layout->addWidget(mp_layer_lineEdit);
    layout->addWidget(select_label);
    layout->addWidget(mp_gds_info_comboBox);

    setLayout(layout);
    setFixedSize(500, 40);

}

void LayerSelectItemWidget::fillData()
{
    mp_layer_lineEdit->setText(m_layer_name);

    for(auto i = 0; i < m_gds_list.size(); i++)
    {
        mp_gds_info_comboBox->addItem(QIcon(":/img/tc_pdk.png"), m_gds_list.at(i));
    }
}

}
