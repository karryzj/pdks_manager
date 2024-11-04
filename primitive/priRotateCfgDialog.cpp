#include "priRotateCfgDialog.h"
#include "ui_priRotateCfgDialog.h"
#include <QDoubleValidator>

PriRotateCfgDialog::PriRotateCfgDialog(QWidget *parent)
    : QDialog(parent), m_rotate_cfg{"0", 0, 0}
    , ui(new Ui::PriRotateCfgDialog)
{
    ui->setupUi(this);

    ui->radio_by->setChecked(true);
    ui->radio_include->setChecked(true);

    // 创建 QDoubleValidator 并设置范围和精度
    // QDoubleValidator *doubleValidator = new QDoubleValidator(this);
    // doubleValidator->setDecimals(3); // 设置小数点后最多3位
    // doubleValidator->setNotation(QDoubleValidator::StandardNotation);
    // ui->lineEdit_degrees->setValidator(doubleValidator);
    ui->lineEdit_degrees->setText("90");
    setWindowTitle(tr("Rotate Config"));
}

PriRotateCfgDialog::~PriRotateCfgDialog()
{
    delete ui;
}

RotateCfg PriRotateCfgDialog::get_result()
{
    return m_rotate_cfg;
}

RotateCfg PriRotateCfgDialog::get_old_result()
{
    return m_old_rotate_cfg;
}

void PriRotateCfgDialog::on_buttonBox_accepted()
{
    m_old_rotate_cfg = m_rotate_cfg;
    m_rotate_cfg.degrees = ui->lineEdit_degrees->text();
    if (ui->radio_by->isChecked())
    {
        m_rotate_cfg.by_to = 0;
    }
    else
    {
        m_rotate_cfg.by_to = 1;
    }
    if (ui->radio_include->isChecked())
    {
        m_rotate_cfg.child_self = 0;
    }
    else
    {
        m_rotate_cfg.child_self = 1;
    }

    accept();
}


void PriRotateCfgDialog::on_buttonBox_rejected()
{
    reject();
}

