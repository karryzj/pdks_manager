#ifndef PRIRECTANGLEACTION_H
#define PRIRECTANGLEACTION_H

#include <PriShapeActionBase.h>

namespace pr
{
class PriRectangleAction final : public PriShapeActionBase
{
    friend class PriShapeAction;
private:
    explicit PriRectangleAction(const QIcon &icon, const QString &text, PrimitiveWindow *parent,ly::LayerWidget* layer_widget);
public:
    ~PriRectangleAction() override;
};
}

#endif // PRIRECTANGLEACTION_H
