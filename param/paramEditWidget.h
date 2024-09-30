#ifndef PARAMEDITWIDGET_H
#define PARAMEDITWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>

#include "pmCommon.h"

namespace mu
{
class Parser;
}
namespace pm
{
class ParamMgr;
class ParamDecl;

class PM_PUBLIC ParamEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParamEditWidget(ParamMgr *param_mgr, QWidget *parent = nullptr);
    ~ParamEditWidget();

    void update_all_items();

private:
    QTableWidget *mp_table_widget;
    QPushButton  *mp_new_button;
    QPushButton  *mp_del_button;
    ParamMgr     *mp_param_mgr;
    ParamDecl    *mp_param_decl;
    bool has_variable(mu::Parser &parser, const QString &expr, const QString &name);

signals:
    void param_changed();
    void param_changed_with_key(const QString &);
public slots:
    void on_add_new_expressison(const QString& name, const QString& expression, const QString& desc);

private slots:
    void addItem();
    void deleteItem();
    void onCellDoubleClicked(int row, int column);
};

}
#endif // PARAMEDITWIDGET_H
