#ifndef TECHRULEITEMWIDGET_H
#define TECHRULEITEMWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

namespace pdk
{
class TechRuleItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TechRuleItemWidget(const QString &tech_rule, QWidget *parent = nullptr);
    void fill_data();
    QString get_result();
    QString get_input_result();

    QString tech_rule() const;
    void set_tech_rule(const QString &tech_rule);

private:
    void setup_ui();

    QLineEdit       *mp_rule_lineEdit;
    QPushButton     *mp_modify_button;
    QPushButton     *mp_del_button;
    QString         m_tech_rule;
    QString         m_old_rule;
private slots:
    void modify_rule_info();
    void del_rule_info();
signals:
    void delete_rule_item(TechRuleItemWidget *widget);
    void modify_rule_item(QString text);
};

}
#endif // TECHRULEITEMWIDGET_H
