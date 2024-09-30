#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QFormLayout>
#include <QDoubleValidator>
#include <ParamValidator.h>
#include <ParamUtils.h>
#include "paramEditDialog.h"
#include "paramDecl.h"

namespace pm
{

ParamEditDialog::ParamEditDialog(QWidget *parent) : QDialog(parent),
    mp_param_decl(new ParamDecl("", 0, "")), mp_name_line_edit(new QLineEdit()),
    mp_var_line_edit(new QLineEdit()), mp_desc_line_edit(new QLineEdit())
{
    auto vaildator = new ParamValidator(mp_var_line_edit);
    mp_var_line_edit->setValidator(vaildator);

    setWindowTitle(tr("参数编辑"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    QLabel *label_name = new QLabel(tr("名称:"));
    label_name->setBuddy(mp_name_line_edit);
    // 正则表达式：以字母或下划线开头，后面可以跟字母、数字或下划线
    QRegularExpression regex("^[a-zA-Z_][a-zA-Z0-9_]*$");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex, this);
    mp_name_line_edit->setValidator(validator);

    QLabel *label_var = new QLabel(tr("默认值:"));
    label_var->setBuddy(mp_var_line_edit);
    // 创建 QDoubleValidator 并设置范围和精度
    // QDoubleValidator *doubleValidator = new QDoubleValidator(this);
    // doubleValidator->setDecimals(3); // 设置小数点后最多3位
    // doubleValidator->setNotation(QDoubleValidator::StandardNotation);
    // mp_var_line_edit->setValidator(doubleValidator);
    QLabel *label_desc = new QLabel(tr("描述:"));
    label_desc->setBuddy(mp_desc_line_edit);
    // mp_desc_line_edit->setValidator(validator);

    formLayout->addRow(label_name, mp_name_line_edit);
    formLayout->addRow(label_var, mp_var_line_edit);
    formLayout->addRow(label_desc, mp_desc_line_edit);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    layout->addLayout(formLayout);
    layout->addLayout(buttonLayout);
    mp_name_line_edit->setFocus();

    connect(okButton, &QPushButton::clicked, this, &ParamEditDialog::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

ParamEditDialog::ParamEditDialog(const QStringList &content, QWidget *parent) : QDialog(parent),
    mp_param_decl(new ParamDecl("", 0, "")), mp_name_line_edit(new QLineEdit()),
    mp_var_line_edit(new QLineEdit()), mp_desc_line_edit(new QLineEdit()), m_content(content)
{
    auto vaildator = new ParamValidator(mp_var_line_edit);
    mp_var_line_edit->setValidator(vaildator);

    setWindowTitle(tr("参数编辑"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    QLabel *label_name = new QLabel(tr("名称:"));
    label_name->setBuddy(mp_name_line_edit);
    mp_name_line_edit->setEnabled(false);
    QRegularExpression regex("^[a-zA-Z_][a-zA-Z0-9_]*$");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex, this);
    mp_name_line_edit->setValidator(validator);
    mp_name_line_edit->setText(m_content.at(0));
    QLabel *label_var = new QLabel(tr("默认值:"));
    label_var->setBuddy(mp_var_line_edit);
    // 创建 QDoubleValidator 并设置范围和精度
    // QDoubleValidator *doubleValidator = new QDoubleValidator(this);
    // doubleValidator->setDecimals(3); // 设置小数点后最多3位
    // doubleValidator->setNotation(QDoubleValidator::StandardNotation);
    // mp_var_line_edit->setValidator(doubleValidator);
    mp_var_line_edit->setText(ParamUtils::str_to_3_pre_str(m_content.at(1)));
    QLabel *label_desc = new QLabel(tr("描述:"));
    label_desc->setBuddy(mp_desc_line_edit);
    // mp_desc_line_edit->setValidator(validator);
    if (m_content.size() > 2)
        mp_desc_line_edit->setText(m_content.at(2));
    // else
    //     mp_desc_line_edit->setText("");

    formLayout->addRow(label_name, mp_name_line_edit);
    formLayout->addRow(label_var, mp_var_line_edit);
    formLayout->addRow(label_desc, mp_desc_line_edit);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    layout->addLayout(formLayout);
    layout->addLayout(buttonLayout);
    mp_name_line_edit->setFocus();

    connect(okButton, &QPushButton::clicked, this, &ParamEditDialog::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

ParamEditDialog::~ParamEditDialog()
{
    delete mp_name_line_edit;
    delete mp_var_line_edit;
    delete mp_desc_line_edit;
}

void ParamEditDialog::onOkClicked()
{
    QString name_content = mp_name_line_edit->text();
    QString var_content = mp_var_line_edit->text();
    if (name_content.isEmpty() || var_content.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "Content cannot be empty!");
    }
    else
    {
        m_content.clear();
        m_content.append(name_content);
        m_content.append(var_content);
        if (!mp_desc_line_edit->text().isEmpty())
            m_content.append(mp_desc_line_edit->text());
        accept();
    }
}

QStringList ParamEditDialog::getContent() const
{
    return m_content;
}

}
