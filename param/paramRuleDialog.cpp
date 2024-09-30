#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QFormLayout>
#include <QDoubleValidator>
#include "expression.h"
#include "paramRuleDialog.h"

namespace pm
{

ParamRuleDialog::ParamRuleDialog(QWidget *parent)
    : QDialog(parent), mp_rule_line_edit(new QLineEdit())
{
    setWindowTitle(tr("参数编辑"));

    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    layout->addWidget(mp_rule_line_edit);
    layout->addLayout(buttonLayout);
    mp_rule_line_edit->setFocus();

    connect(okButton, &QPushButton::clicked, this, &ParamRuleDialog::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

ParamRuleDialog::ParamRuleDialog(const QString &content, QWidget *parent)
    : QDialog(parent), mp_rule_line_edit(new QLineEdit()), m_content(content)
{
    setWindowTitle(tr("参数编辑"));

    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mp_rule_line_edit->setText(m_content);

    layout->addWidget(mp_rule_line_edit);
    layout->addLayout(buttonLayout);
    mp_rule_line_edit->setFocus();

    connect(okButton, &QPushButton::clicked, this, &ParamRuleDialog::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

ParamRuleDialog::~ParamRuleDialog()
{
    delete mp_rule_line_edit;
}

void ParamRuleDialog::onOkClicked()
{
    m_content = mp_rule_line_edit->text();
    if (m_content.isEmpty() || !Expression::is_bool(m_content))
    {
        QMessageBox::warning(this, "Warning", "Content Invalid!");
    }
    else
    {
        accept();
    }
}

QString ParamRuleDialog::getContent() const
{
    return m_content;
}

}
