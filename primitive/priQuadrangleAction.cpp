#include "priQuadrangleAction.h"

using namespace pr;

PriQuardangleAction::PriQuardangleAction(const QIcon &icon, const QString &text, PrimitiveWindow *parent,ly::LayerWidget* layer_widget)
    :PriShapeActionBase{icon,text,parent,true,true,true,layer_widget}
{

}

PriQuardangleAction::~PriQuardangleAction()
{

}
