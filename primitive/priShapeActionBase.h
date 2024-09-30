#ifndef PRISHAPEACTIONBASE_H
#define PRISHAPEACTIONBASE_H

#include <PriShapeDialog.h>
#include <QAction>
#include <qdialog.h>

namespace ly
{
class LayerWidget;
}

namespace pr
{
class PrimitiveWindow;
class PriShapeActionBase : public QAction
{
protected:
    PriShapeActionBase(const QIcon &icon,
                       const QString &text,
                       PrimitiveWindow *parent,
                       bool add_type,
                       bool add_direction,
                       bool add_layer,
                       ly::LayerWidget* layer_widget);
public:
    ~PriShapeActionBase() override;
public:
    PriShapeDialog* ui_form();
private:
    QString m_shape_name;
    bool m_add_type;
    bool m_add_direction;
    bool m_add_layer;
    ly::LayerWidget* mp_layer_widget;
    PriShapeDialog* mp_ui = nullptr;
    PrimitiveWindow* mp_mw;
};
}


#endif // PRISHAPEACTIONBASE_H
