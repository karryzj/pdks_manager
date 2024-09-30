#include "priRectangleAction.h"

using namespace pr;

PriRectangleAction::PriRectangleAction(const QIcon &icon, const QString &text, PrimitiveWindow *parent,ly::LayerWidget* layer_widget)
:PriShapeActionBase{icon,text,parent,true,true,true,layer_widget}
{

}

PriRectangleAction::~PriRectangleAction()
{

}
