#ifndef DRCMGRITEMWIDGET_H
#define DRCMGRITEMWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

namespace pdk
{

class DrcMgrItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrcMgrItemWidget(QWidget *parent = nullptr);

private:
    void setup_ui();
    void fill_data();

    QLineEdit       *mp_drc_lineEdit;
    QPushButton     *mp_modify_button;
    QPushButton     *mp_del_button;

    QString m_tech_rule;
signals:
};
}
#endif // DRCMGRITEMWIDGET_H
