#ifndef DRCMGRWIDGET_H
#define DRCMGRWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>

namespace pdk
{

class DrcMgrWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrcMgrWidget(QWidget *parent = nullptr);

private:

    void setup_ui();
    void fill_data();

    QListWidget         *mp_list_widget;
    QPushButton         *mp_add_button;
    QStringList         m_drc_list;
signals:
};

}
#endif // DRCMGRWIDGET_H
