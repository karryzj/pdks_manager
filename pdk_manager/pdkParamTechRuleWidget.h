#ifndef PDKPARAMTECHRULEWIDGET_H
#define PDKPARAMTECHRULEWIDGET_H

// #include <QStandardItem>
#include <QListWidget>
#include <QWidget>
namespace pdk
{

class PdkParamTechRuleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PdkParamTechRuleWidget(QWidget *parent = nullptr);

private:
    void setupUi();
    void fillData();
    QListWidget         *mp_list_widget;
    // QStandardItemModel  *mp_model;
signals:
};

}
#endif // PDKPARAMTECHRULEWIDGET_H
