#include "priPolygenAction.h"

using namespace pr;

PriPolygenAction::PriPolygenAction(const QIcon &icon, const QString &text, PrimitiveWindow *parent, ly::LayerWidget* layer_widget)
    : PriShapeActionBase{icon, text, parent, true, false, true, layer_widget}
{
}

PriPolygenAction::~PriPolygenAction()
{
}
