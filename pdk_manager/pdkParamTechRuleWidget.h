#ifndef PDKPARAMTECHRULEWIDGET_H
#define PDKPARAMTECHRULEWIDGET_H

// #include <QStandardItem>
#include <QListWidget>
#include <QWidget>
namespace pdk
{
class PdkInstance;
class PdkParamTechRuleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PdkParamTechRuleWidget(PdkInstance *pdk_instance, QWidget *parent = nullptr);
    void fillData();
    void clear_view();

private:
    void setupUi();
    PdkInstance         *mp_pdk_instance;
    QListWidget         *mp_list_widget;
    // QStandardItemModel  *mp_model;
signals:
};

}
#endif // PDKPARAMTECHRULEWIDGET_H
