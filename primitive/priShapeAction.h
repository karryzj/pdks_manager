#ifndef PRISHAPEACTION_H
#define PRISHAPEACTION_H

#include <QAction>

class QObject;

namespace ly
{
class LayerWidget;
}

namespace pr
{
class PriShapeActionBase;
class PrimitiveWindow;
class PriShapeDialog;
class PriShapeAction final : public QAction
{
public:
    PriShapeAction(const QIcon& icon, const QString& text, PrimitiveWindow* parent, ly::LayerWidget* layer_widget);
    ~PriShapeAction();

    PriShapeDialog* ui_form();

private:
    PriShapeActionBase* mp_action;
};
}

#endif // PRISHAPEACTION_H
