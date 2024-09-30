#ifndef PRITRIANGLEACTION_H
#define PRITRIANGLEACTION_H

#include <PriShapeActionBase.h>

namespace pr
{
class PriTriangleAction final : public PriShapeActionBase
{
    friend class PriShapeAction;
private:
    explicit PriTriangleAction(const QIcon &icon, const QString &text, PrimitiveWindow *parent,ly::LayerWidget* layer_widget);
public:
    ~PriTriangleAction() override;
};
}

#endif // PRITRIANGLEACTION_H
