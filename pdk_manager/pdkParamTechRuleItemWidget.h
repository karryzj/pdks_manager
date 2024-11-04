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
    QString get_result();
    void set_express(const QString &exp);
    void fill_data();

private:
    void setup_ui();

    QLineEdit       *mp_value_lineEdit;
    QLabel          *mp_name_label;
    QLabel          *mp_desc_label;
    pm::ParamDecl   *mp_param_decl;

private slots:
    void changed_value();

signals:
    void modify_param_item(QString text);
};
}

#endif // PDKPARAMTECHRULEITEMWIDGET_H
