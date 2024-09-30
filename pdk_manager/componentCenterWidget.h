#ifndef COMPONENTCENTERWIDGET_H
#define COMPONENTCENTERWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <QToolButton>

namespace pdk
{
class PriModeWidget;
class GdsModeWidget;

class ComponentCenterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ComponentCenterWidget(QWidget *parent = nullptr);

private:
    void setupUi();

    QStackedWidget      *mp_stackedWidget;
    QToolButton         *mp_switchButton;
    PriModeWidget       *mp_pri_mode_widget;
    GdsModeWidget       *mp_gds_mode_widget;
    bool m_isOn;


private slots:
    void toggleMode();
signals:
};

}
#endif // COMPONENTCENTERWIDGET_H
