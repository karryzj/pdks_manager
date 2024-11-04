#ifndef PRIDOCKWIDGET_H
#define PRIDOCKWIDGET_H

#include <QDockWidget>

namespace pr
{
class PrimitiveWindow;
class PriDockWidget final: public QDockWidget
{
    Q_OBJECT
public:
    PriDockWidget(const QString &title, PrimitiveWindow *parent, Qt::WindowFlags flags = Qt::WindowFlags());
    ~PriDockWidget() override;
signals:
    void dock_widget_resize();

private:
    void resizeEvent(QResizeEvent* event) override;
};
}


#endif // PRIDOCKWIDGET_H
