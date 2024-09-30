#include "priShapeActionBase.h"
#include "primitiveDefines.h"
#include "shapeDefines.h"
#include <PriShapeDialog.h>
#include <PrimitiveWindow.h>
#include <ShapeDecl.h>
#include <ShapeFactory.h>
#include <layWidget.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtoolbar.h>
#include "priShapeDialog.h"

using namespace pr;

PriShapeActionBase::PriShapeActionBase(const QIcon &icon,
                                       const QString &shape_name,
                                       PrimitiveWindow *parent,
                                       bool add_type,
                                       bool add_direction,
                                       bool add_layer,
                                       ly::LayerWidget* layer_widget)
    : QAction{icon, shape_name, parent}
    , m_shape_name(shape_name)
    , m_add_type(add_type)
    , m_add_direction(add_direction)
    , m_add_layer(add_layer)
    , mp_layer_widget(layer_widget)
    , mp_mw(parent)
{

}

PriShapeActionBase::~PriShapeActionBase()
{
    if(mp_ui)
    {
        delete mp_ui;
    }
}

PriShapeDialog* PriShapeActionBase::ui_form()
{
    if(mp_ui)
    {
        delete mp_ui;
    }
    mp_ui = new PriShapeDialog(m_shape_name, m_add_type, m_add_direction, m_add_layer, mp_layer_widget, mp_mw->current_primitive());
    mp_ui->update_all();
    return mp_ui;
}
