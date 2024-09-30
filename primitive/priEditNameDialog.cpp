#include "priEditNameDialog.h"
#include "ui_priEditNameDialog.h"

PriEditNameDialog::PriEditNameDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PriEditNameDialog)
{
    ui->setupUi(this);
    ui->combo_box_type->addItem(tr("附着树类型"));
    ui->combo_box_type->addItem(tr("Python类型"));
    set_type_visible(false);
}

PriEditNameDialog::PriEditNameDialog(const QString &name, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PriEditNameDialog)
{
    ui->setupUi(this);
    ui->combo_box_type->addItem(tr("附着树类型"));
    ui->combo_box_type->addItem(tr("Python类型"));
    set_type_visible(false);
    ui->line_edit_name->setText(name);
}

PriEditNameDialog::~PriEditNameDialog()
{
    delete ui;
}

QString PriEditNameDialog::getContent() const
{
    return ui->line_edit_name->text();
}

void PriEditNameDialog::set_type_visible(bool is_visible)
{
    ui->label_type->setVisible(is_visible);
    ui->combo_box_type->setVisible(is_visible);
}

int PriEditNameDialog::get_type() const
{
    return ui->combo_box_type->currentIndex();
}
