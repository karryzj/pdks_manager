#include "priDockWidget.h"
#include <PrimitiveWindow.h>

using namespace pr;

PriDockWidget::PriDockWidget(const QString &title, PrimitiveWindow *parent, Qt::WindowFlags flags)
    : QDockWidget(title, parent, flags)
{

}

PriDockWidget::~PriDockWidget()
{

}

void PriDockWidget::resizeEvent(QResizeEvent *event)
{
    QDockWidget::resizeEvent(event);
    emit dock_widget_resize();
}
