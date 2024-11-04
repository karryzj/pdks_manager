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
class PdkInstance;
class PrimitiveInfoWidget;
class PriModeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PriModeWidget(PdkInstance *pdk_instance, QWidget *parent = nullptr);
    ~PriModeWidget();
    void set_disable_primitive_relation(bool enable);
    void fillUiData();
    void clear_view();
private:
    void setupUi();
    void fillData();
    bool removeDir(const QString &dirPath);
    bool copyDirContents(const QString &srcPath, const QString &dstPath);

private slots:
    void fillPriName(const QString &text);
    // void fillCellData(const QString &text);
    void fillCellData(int idx);

private:
    PdkInstance         *mp_pdk_instance;
    QComboBox           *mp_pri_lib_combo_box;
    QComboBox           *mp_pri_name_combo_box;
    LayerSelectWidget   *mp_layer_select_widget;
    PdkParamTechRuleWidget  *mp_pdk_param_tech_rule_widget;
    TechRuleWidget      *mp_tech_rule_widget;
    PrimitiveInfoWidget *mp_pri_info_widget;

signals:
};

}
#endif // PRIMODEWIDGET_H
