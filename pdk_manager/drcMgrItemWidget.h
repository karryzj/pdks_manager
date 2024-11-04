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
    explicit DrcMgrItemWidget(const QString drc, QWidget *parent = nullptr);

private:
    void setup_ui();
    void fill_data();

    QLineEdit       *mp_drc_lineEdit;
    QPushButton     *mp_modify_button;
    QPushButton     *mp_del_button;
    QString         m_drc;

private slots:
    void modify_drc_info();
    void del_drc_info();
signals:
    void delete_drc_item(DrcMgrItemWidget *widget);
};
}
#endif // DRCMGRITEMWIDGET_H
