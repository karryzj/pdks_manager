#ifndef PRICIRCLEACTION_H
#define PRICIRCLEACTION_H

#include <PriShapeActionBase.h>

namespace pr
{
class PriCircleAction final : public PriShapeActionBase
{
    friend class PriShapeAction;
private:
    explicit PriCircleAction(const QIcon &icon, const QString &text, PrimitiveWindow *parent,ly::LayerWidget* layer_widget);
public:
    ~PriCircleAction() override;
};
}

#endif // PRICIRCLEACTION_H
