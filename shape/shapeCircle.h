#ifndef SHAPECIRCLE_H
#define SHAPECIRCLE_H

#include "shapeBase.h"
#include "shapeFactoryBase.h"

namespace sp
{

// shape param setting and produce graphics
class SP_PUBLIC ShapeCircle final: public ShapeBase
{
public:
    ShapeCircle(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl>& params, ShapePointGraphicsItem* parent_attach_point);
    ~ShapeCircle() override;

protected:
    void update_variables() override;
    void update_attach_pointes() override;
    QPainterPath build_path() override;

private:
    mutable qreal m_radius;
    mutable std::string m_radius_exp;
};

class ShapeCircleFacotry final : public ShapeFactoryBase
{
public:
    ShapeCircleFacotry();
    ~ShapeCircleFacotry() override;

    ShapeBase *create_shape(const QString &shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const override;
};

}

#endif // SHAPECIRCLE_H
