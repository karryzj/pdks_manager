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

private:
    void setup_ui();
    void fill_data();

    QLineEdit       *mp_rule_lineEdit;
    QPushButton     *mp_modify_button;
    QPushButton     *mp_del_button;

    QString m_tech_rule;
signals:
};

}
#endif // TECHRULEITEMWIDGET_H
