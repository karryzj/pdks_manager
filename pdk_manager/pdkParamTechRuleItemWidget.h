#ifndef PDKPARAMTECHRULEITEMWIDGET_H
#define PDKPARAMTECHRULEITEMWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>

namespace pm
{
class ParamDecl;
}
namespace pdk
{

class PdkParamTechRuleItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PdkParamTechRuleItemWidget(pm::ParamDecl *param_decl, QWidget *parent = nullptr);

private:
    void setup_ui();
    void fill_data();
    QLineEdit       *mp_value_lineEdit;
    QLabel          *mp_name_label;
    QLabel          *mp_desc_label;
    pm::ParamDecl   *mp_param_decl;
};
}

#endif // PDKPARAMTECHRULEITEMWIDGET_H
