#ifndef PDKLAYERMGRWIDGET_H
#define PDKLAYERMGRWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>

namespace pdk
{
class PdkInstance;
class PdkLayerMgrWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PdkLayerMgrWidget(PdkInstance *pdk_instance, QWidget *parent = nullptr);
    void fill_data();

private:
    void setup_ui();

    PdkInstance         *mp_pdk_instance;
    QListWidget         *mp_list_widget;
    QPushButton         *mp_add_button;
    QStringList         m_rule_list;

private slots:
    void add_gds_layer();
    void del_current_item();

signals:
};
}
#endif // PDKLAYERMGRWIDGET_H
