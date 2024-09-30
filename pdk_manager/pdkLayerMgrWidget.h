#ifndef PDKLAYERMGRWIDGET_H
#define PDKLAYERMGRWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>

namespace pdk
{
class PdkLayerMgrWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PdkLayerMgrWidget(QWidget *parent = nullptr);

private:
    void setup_ui();
    void fill_data();

    QListWidget         *mp_list_widget;
    QPushButton         *mp_add_button;
    QStringList         m_rule_list;

signals:
};
}
#endif // PDKLAYERMGRWIDGET_H
