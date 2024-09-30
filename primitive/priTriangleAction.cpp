#include "priTriangleAction.h"

using namespace pr;

PriTriangleAction::PriTriangleAction(const QIcon &icon, const QString &text, PrimitiveWindow *parent,ly::LayerWidget* layer_widget)
    :PriShapeActionBase{icon,text,parent,true,true,true,layer_widget}
{

}

PriTriangleAction::~PriTriangleAction()
{

}
