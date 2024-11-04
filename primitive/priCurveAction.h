#ifndef PRICURVEACTION_H
#define PRICURVEACTION_H

#include <PriShapeActionBase.h>

namespace pr
{
class PriCurveAction final : public PriShapeActionBase
{
    friend class PriShapeAction;
private:
    explicit PriCurveAction(const QIcon &icon, const QString &text, PrimitiveWindow *parent, ly::LayerWidget* layer_widget);
public:
    ~PriCurveAction() override;
};
}

#endif // PRICURVEACTION_H
