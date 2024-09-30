#ifndef PARAMRULEDIALOG_H
#define PARAMRULEDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QLineEdit>

namespace pm
{
class ParamRuleDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ParamRuleDialog(QWidget *parent = nullptr);
    ParamRuleDialog(const QString &content, QWidget *parent = nullptr);
    ~ParamRuleDialog();
    QString getContent() const;

private:
    QLineEdit *mp_rule_line_edit;
    QString m_content;

private slots:
    void onOkClicked();
};

}
#endif // PARAMRULEDIALOG_H
