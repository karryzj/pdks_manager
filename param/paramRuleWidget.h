#ifndef PARAMRULEWIDGET_H
#define PARAMRULEWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include "pmCommon.h"

namespace pm
{

class ParamMgr;

class PM_PUBLIC ParamRuleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParamRuleWidget(ParamMgr *param_mgr, QWidget *parent = nullptr);

    ~ParamRuleWidget();

    void update_all_items();

private:
    QTableWidget *mp_table_widget;
    QPushButton  *mp_new_button;
    QPushButton  *mp_del_button;
    ParamMgr     *mp_param_mgr;

private slots:
    void addItem();
    void deleteItem();
    void onCellDoubleClicked(int row, int column);
};
}
#endif // PARAMRULEWIDGET_H
