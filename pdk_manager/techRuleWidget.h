#ifndef TECHRULEWIDGET_H
#define TECHRULEWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>

namespace pdk
{
class PdkInstance;

class TechRuleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TechRuleWidget(PdkInstance *pdk_instance, QWidget *parent = nullptr);
    void fill_data();
    void clear_view();

private:
    void setup_ui();

    PdkInstance         *mp_pdk_instance;
    QListWidget         *mp_list_widget;
    QPushButton         *mp_add_button;
    QStringList         m_rule_list;

private slots:
    void add_rule_item();
signals:
};

}
#endif // TECHRULEWIDGET_H
