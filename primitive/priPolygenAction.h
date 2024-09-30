#ifndef PRIPOLYGENACTION_H
#define PRIPOLYGENACTION_H

#include <PriShapeActionBase.h>

namespace pr
{
class PriPolygenAction final : public PriShapeActionBase
{
    friend class PriShapeAction;
private:
    explicit PriPolygenAction(const QIcon &icon, const QString &text, PrimitiveWindow *parent, ly::LayerWidget* layer_widget);
public:
    ~PriPolygenAction() override;
};
}


#endif // PRIPOLYGENACTION_H
