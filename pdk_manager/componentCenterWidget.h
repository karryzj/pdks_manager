#ifndef COMPONENTCENTERWIDGET_H
#define COMPONENTCENTERWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <QToolButton>

namespace pdk
{
class PriModeWidget;
class GdsModeWidget;
class PdkInstance;

class ComponentCenterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ComponentCenterWidget(PdkInstance *mp_pdk_instance, QWidget *parent = nullptr);
    PriModeWidget* get_pri_mode_widget();
    GdsModeWidget* get_gds_mode_widget();
    void set_switch_enable(bool enable);
    void set_switch_mode(int mode);
    void fill_data();

private:
    void setupUi();

    PdkInstance         *mp_pdk_instance;
    QStackedWidget      *mp_stackedWidget;
    QToolButton         *mp_switchButton;
    PriModeWidget       *mp_pri_mode_widget;
    GdsModeWidget       *mp_gds_mode_widget;
    QToolButton         *mp_pri_save_button;
    QToolButton         *mp_export_gds_button;

    bool m_isOn;


private slots:
    void toggleMode();
    void savePrimitive();
    void exportGds();
signals:
};

}
#endif // COMPONENTCENTERWIDGET_H
