#include "priSectorAction.h"

using namespace pr;

PriSectorAction::PriSectorAction(const QIcon &icon, const QString &text, PrimitiveWindow *parent,ly::LayerWidget* layer_widget)
:PriShapeActionBase{icon,text,parent,true,false,true,layer_widget}
{

}

PriSectorAction::~PriSectorAction()
{

}
