#ifndef GDSMODEWIDGET_H
#define GDSMODEWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

namespace pdk
{
class PdkInstance;
class GdsModeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GdsModeWidget(PdkInstance *pdk_instance, QWidget *parent = nullptr);
    ~GdsModeWidget();
    void fill_data();
    void clear_view();

private:
    PdkInstance         *mp_pdk_instance;
    QPushButton         *mp_open_gds_button;
    QLabel              *mp_thumbnail_label;
    QLabel              *mp_file_name_label;

    void setupUi();

private slots:
    void open_file_dialog();
signals:
};

}

#endif // GDSMODEWIDGET_H
