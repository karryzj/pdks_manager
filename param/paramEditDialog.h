#ifndef PARAMEDITDIALOG_H
#define PARAMEDITDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QLineEdit>
#include <QStringList>

namespace pm
{

class ParamDecl;

class ParamEditDialog : public QDialog
{
    Q_OBJECT
public:
    ParamEditDialog(QWidget *parent = nullptr);
    explicit ParamEditDialog(const QStringList &content, QWidget *parent = nullptr);
    ~ParamEditDialog();
    QStringList getContent() const;

private:
    QLineEdit *mp_name_line_edit;
    QLineEdit *mp_var_line_edit;
    QLineEdit *mp_desc_line_edit;
    QStringList m_content;
    ParamDecl *mp_param_decl;

private slots:
    void onOkClicked();
};

}
#endif // PARAMEDITDIALOG_H
