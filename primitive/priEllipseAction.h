#ifndef PRIELLIPSEACTION_H
#define PRIELLIPSEACTION_H

#include <PriShapeActionBase.h>

namespace pr
{
class PriEllipseAction final : public PriShapeActionBase
{
    friend class PriShapeAction;
private:
    explicit PriEllipseAction(const QIcon &icon, const QString &text, PrimitiveWindow* parent,ly::LayerWidget* layer_widget);
public:
    ~PriEllipseAction() override;
};
}

#endif // PRIELLIPSEACTION_H
