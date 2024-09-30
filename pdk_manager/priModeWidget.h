#ifndef PRIMODEWIDGET_H
#define PRIMODEWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QTableView>
#include <QStandardItemModel>
#include <QToolButton>

namespace pdk
{
class LayerSelectWidget;
class PdkParamTechRuleWidget;
class TechRuleWidget;
class PriModeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PriModeWidget(QWidget *parent = nullptr);
    ~PriModeWidget();
private:
    void setupUi();

private:
    QComboBox           *mp_pri_lib_combo_box;
    QComboBox           *mp_pri_name_combo_box;
    QTableView          *mp_pri_select_table_view;
    QStandardItemModel  *mp_pri_model;
    QToolButton         *mp_pri_save_button;
    QToolButton         *mp_export_gds_button;
    LayerSelectWidget   *mp_layer_select_widget;
    PdkParamTechRuleWidget  *mp_pdk_param_tech_rule_widget;
    TechRuleWidget      *mp_tech_rule_widget;
signals:
};

}
#endif // PRIMODEWIDGET_H
