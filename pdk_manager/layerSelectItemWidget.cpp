#include <QHBoxLayout>
#include <QLabel>
#include "pdkInstance.h"
#include "gdsLayerInfo.h"
#include "layerSelectItemWidget.h"

namespace pdk
{

CustomComboBox::CustomComboBox(QWidget *parent)
    : QComboBox(parent)
{
}

void CustomComboBox::showPopup()
{
    emit custom_showPopup();
    QComboBox::showPopup();
}

LayerSelectItemWidget::LayerSelectItemWidget(PdkInstance *pdk_instance, const QString &layer, const QString &gds_layer, QWidget *parent)
    : QWidget(parent), mp_pdk_instance(pdk_instance), m_layer_name(layer), m_gds_layer(gds_layer),
      mp_layer_lineEdit(new QLineEdit()), mp_gds_info_comboBox(new CustomComboBox())
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

    mp_layer_lineEdit->setEnabled(false);
    mp_layer_lineEdit->setFixedSize(200, 30);
    // 使用样式表设置左边的图标
    mp_layer_lineEdit->setStyleSheet("QLineEdit { padding-left: 30px; background: url(:img/tc_ty.png) no-repeat; "
                                     "background-color: #1e1e1e; color: white;  }");

    QLabel *select_label = new QLabel();
    QPixmap pixmap(":/img/ty_pdk.png");  // 使用资源中的图片
    select_label->setPixmap(pixmap);  // 设置图标
    select_label->setAlignment(Qt::AlignCenter);  // 居中对齐
    select_label->setFixedSize(30, 20);
    mp_gds_info_comboBox->setFixedSize(200, 30);
    mp_gds_info_comboBox->setStyleSheet("QComboBox { background-color: #1e1e1e; color: white;  }");

    connect(mp_gds_info_comboBox, &CustomComboBox::custom_showPopup, this, &LayerSelectItemWidget::update_comboBox);
    connect(mp_gds_info_comboBox, &CustomComboBox::currentTextChanged, this, &LayerSelectItemWidget::gds_layer_changed);

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
    mp_gds_info_comboBox->clear();
    auto gds_layers = mp_pdk_instance->pdk_gds_layers();
    m_gds_list.clear();
    for (const auto layer : gds_layers)
    {
        m_gds_list.append(layer->layer_name);
    }

    for(auto i = 0; i < m_gds_list.size(); i++)
    {
        mp_gds_info_comboBox->addItem(QIcon(":/img/tc_pdk.png"), m_gds_list.at(i));
    }
    int idx = mp_gds_info_comboBox->findText(m_gds_layer);
    mp_gds_info_comboBox->setCurrentIndex(idx);
    // mp_gds_info_comboBox->setCurrentText(m_gds_layer);
}

void LayerSelectItemWidget::gds_layer_changed(const QString &text)
{
    QString pri_layer = mp_layer_lineEdit->text();
    QString gds_layer = mp_gds_info_comboBox->currentText();

    auto it = std::find_if(mp_pdk_instance->pdk_gds_layers().begin(), mp_pdk_instance->pdk_gds_layers().end(), [gds_layer](auto tmp_layer)
    {
        return tmp_layer->layer_name == gds_layer;
    });
    if (it != mp_pdk_instance->pdk_gds_layers().end())
    {
        mp_pdk_instance->set_gds_layer_info(pri_layer, *it);
    }

}

void LayerSelectItemWidget::update_comboBox()
{
    auto gds_layers = mp_pdk_instance->pdk_gds_layers();
    m_gds_list.clear();
    for (const auto layer : gds_layers)
    {
        m_gds_list.append(layer->layer_name);
    }

    fillData();
}

}
