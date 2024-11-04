#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QIntValidator>
#include "pdkGdsLayerDialog.h"
#include "styleFrame.h"
#include "gdsLayerInfo.h"

namespace pdk
{
PdkGdsLayerDialog::PdkGdsLayerDialog(GdsLayerInfo *gds_layer_info, QWidget *parent)
    : QDialog(parent), mp_gds_layer_info(gds_layer_info)
{
    setWindowTitle(tr("GDS图层管理"));
    setup_ui();
    fill_data();
}

PdkGdsLayerDialog::~PdkGdsLayerDialog()
{

}

void PdkGdsLayerDialog::setup_ui()
{
    QGroupBox *groupbox = new QGroupBox();
    groupbox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    groupbox->setMaximumSize(350, 300);
    // groupbox->setFixedSize(350, 200);
    // 右上
    QLabel *label1 = new QLabel("Layer Name:");
    QLabel *label2 = new QLabel("GDS Number:");
    QLabel *label3 = new QLabel("Data Type:");
    QLabel *label4 = new QLabel("Style:");
    mp_layername_line_edit = new QLineEdit();
    mp_layernum_line_edit = new QLineEdit();
    // 创建 QDoubleValidator 并设置范围和精度
    QIntValidator *intValidator = new QIntValidator(0, INT_MAX, this);
    mp_layernum_line_edit->setValidator(intValidator);
    mp_datatype_line_edit = new QLineEdit();
    mp_datatype_line_edit->setValidator(intValidator);
    mp_style_frame = new StyleFrame();
    mp_style_frame->setMaximumSize(200, 50);
    mp_style_frame->setFixedSize(200, 50);

    mp_buttonBox = new QDialogButtonBox();
    mp_buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    mp_buttonBox->setGeometry(QRect(10, 250, 301, 32));
    mp_buttonBox->setOrientation(Qt::Horizontal);
    mp_buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    connect(mp_buttonBox, &QDialogButtonBox::accepted, this, &PdkGdsLayerDialog::onAccepted);
    connect(mp_buttonBox, &QDialogButtonBox::rejected, this, &PdkGdsLayerDialog::onRejected);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(label1, 0, 0);
    gridLayout->addWidget(mp_layername_line_edit, 0, 2);
    gridLayout->addWidget(label2, 1, 0);
    gridLayout->addWidget(mp_layernum_line_edit, 1, 2);
    gridLayout->addWidget(label3, 2, 0);
    gridLayout->addWidget(mp_datatype_line_edit, 2, 2);
    gridLayout->addWidget(label4, 3, 0);
    gridLayout->addWidget(mp_style_frame, 3, 2);
    gridLayout->addWidget(mp_buttonBox, 4, 0, 1, -1);
    groupbox->setLayout(gridLayout);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(groupbox);
    layout->setAlignment(groupbox, Qt::AlignTop);
    setLayout(layout);
}

void PdkGdsLayerDialog::fill_data()
{
    mp_layername_line_edit->setText(mp_gds_layer_info->layer_name);
    mp_layernum_line_edit->setText(QString::number(mp_gds_layer_info->layer_num));
    mp_datatype_line_edit->setText(QString::number(mp_gds_layer_info->data_type));
    mp_style_frame->setStyle(mp_gds_layer_info->border_style);
}


void PdkGdsLayerDialog::onAccepted()
{
    mp_gds_layer_info->layer_name = mp_layername_line_edit->text();
    mp_gds_layer_info->layer_num = mp_layernum_line_edit->text().toInt();
    mp_gds_layer_info->data_type = mp_datatype_line_edit->text().toInt();
    mp_gds_layer_info->border_style = mp_style_frame->style();
    accept();
}

void PdkGdsLayerDialog::onRejected()
{
    reject();
}
}
