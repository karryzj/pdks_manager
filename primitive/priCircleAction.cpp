#include "priCircleAction.h"
#include "shapeDefines.h"

using namespace pr;

PriCircleAction::PriCircleAction(const QIcon &icon, const QString &text, PrimitiveWindow *parent,ly::LayerWidget* layer_widget)
    : PriShapeActionBase{icon,text,parent,true,false,true,layer_widget}
{

}

pr::PriCircleAction::~PriCircleAction()
{

}
