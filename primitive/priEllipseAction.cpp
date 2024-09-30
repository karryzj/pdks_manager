#include "priEllipseAction.h"

using namespace pr;

PriEllipseAction::PriEllipseAction(const QIcon &icon, const QString &text, PrimitiveWindow *parent,ly::LayerWidget* layer_widget)
    : PriShapeActionBase{icon,text,parent,true,false,true,layer_widget}
{

}

PriEllipseAction::~PriEllipseAction()
{

}
