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
    void fill_data();
    void add_drc_list(const QString &drc);

    QStringList drc_list() const;
    void set_drc_list(const QStringList &drc_list);

private:
    void setup_ui();

    QListWidget         *mp_list_widget;
    QPushButton         *mp_add_button;
    QStringList         m_drc_list;

private slots:
    void add_drc_item();
signals:
};

}
#endif // DRCMGRWIDGET_H
