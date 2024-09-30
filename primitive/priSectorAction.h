#ifndef PRISECTORACTION_H
#define PRISECTORACTION_H

#include <PriShapeActionBase.h>

namespace pr
{
class PriSectorAction final : public PriShapeActionBase
{
    friend class PriShapeAction;
private:
    explicit PriSectorAction(const QIcon &icon, const QString &text, PrimitiveWindow *parent,ly::LayerWidget* layer_widget);
public:
    ~PriSectorAction() override;
};
}

#endif // PRISECTORACTION_H
