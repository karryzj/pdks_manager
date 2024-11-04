#include "priCurveAction.h"

using namespace pr;

PriCurveAction::PriCurveAction(const QIcon &icon, const QString &text, PrimitiveWindow *parent, ly::LayerWidget* layer_widget)
    : PriShapeActionBase{icon, text, parent, true, false, true, true, layer_widget} // HINT@leixunyong。没有方向
{

}

pr::PriCurveAction::~PriCurveAction()
{

}
