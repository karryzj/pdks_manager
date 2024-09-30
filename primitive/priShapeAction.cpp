#include "priQuadrangleAction.h"
#include "priShapeAction.h"
#include "shapeDefines.h"
#include <PriCircleAction.h>
#include <PriEllipseAction.h>
#include <PriPolygenAction.h>
#include <PriRectangleAction.h>
#include <PriSectorAction.h>
#include <PriTriangleAction.h>
#include <PrimitiveWindow.h>

using namespace pr;

PriShapeAction::PriShapeAction(const QIcon &icon, const QString &text, PrimitiveWindow *parent, ly::LayerWidget* layer_widget)
    : QAction(icon, text, parent)
{
    if(text == SHAPE_CIRCLE)
    {
        mp_action = new PriCircleAction(icon, text, parent, layer_widget);
        setObjectName(SHAPE_CIRCLE);
    }
    else if(text == SHAPE_ELLIPSE)
    {
        mp_action = new PriEllipseAction(icon, text, parent, layer_widget);
        setObjectName(SHAPE_ELLIPSE);

    }
    else if(text == SHAPE_QUADRANGLE)
    {
        mp_action = new PriQuardangleAction(icon, text, parent, layer_widget);
        setObjectName(SHAPE_QUADRANGLE);
    }
    else if(text == SHAPE_RECTANGLE)
    {
        mp_action = new PriRectangleAction(icon, text, parent, layer_widget);
        setObjectName(SHAPE_RECTANGLE);
    }
    else if(text == SHAPE_SECTOR)
    {
        mp_action = new PriSectorAction(icon, text, parent, layer_widget);
        setObjectName(SHAPE_SECTOR);
    }
    else if(text == SHAPE_TRIANGLE)
    {
        mp_action = new PriTriangleAction(icon, text, parent, layer_widget);
        setObjectName(SHAPE_TRIANGLE);
    }
    else if(text == SHAPE_POLYGEN)
    {
        mp_action = new PriPolygenAction(icon, text, parent, layer_widget);
        setObjectName(SHAPE_POLYGEN);
    }
    else
    {
        Q_ASSERT(false);  // 程序运行到这里来说明出现了新增图形没有适配好。
    }
}

PriShapeAction::~PriShapeAction()
{
    delete mp_action;
}

PriShapeDialog* PriShapeAction::ui_form()
{
    return mp_action->ui_form();
}
