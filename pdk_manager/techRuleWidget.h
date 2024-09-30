#ifndef TECHRULEWIDGET_H
#define TECHRULEWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>

namespace pdk
{
class TechRuleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TechRuleWidget(QWidget *parent = nullptr);

private:
    void setup_ui();
    void fill_data();

    QListWidget         *mp_list_widget;
    QPushButton         *mp_add_button;
    QStringList         m_rule_list;
signals:
};

}
#endif // TECHRULEWIDGET_H
