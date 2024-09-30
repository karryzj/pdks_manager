#ifndef PRIQUADRANGLEACTION_H
#define PRIQUADRANGLEACTION_H

#include "PriShapeActionBase.h"
namespace pr
{
class PriQuardangleAction final : public PriShapeActionBase
{
    friend class PriShapeAction;
private:
    explicit PriQuardangleAction(const QIcon &icon, const QString &text, PrimitiveWindow *parent,ly::LayerWidget* layer_widget);
public:
    ~PriQuardangleAction() override;
};
}

#endif // PRIQUADRANGLEACTION_H
