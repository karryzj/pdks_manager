#ifndef SHAPETRIANGLE_H
#define SHAPETRIANGLE_H

#include "shapeBase.h"
#include "shapeFactoryBase.h"

namespace pm
{
class ParamMgr;
class PointE;
}

namespace sp
{
class SP_PUBLIC ShapeTriangle final: public ShapeBase
{

public:
    ShapeTriangle(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point);
    ~ShapeTriangle() override;

protected:
    void update_variables() override;
    void update_attach_pointes() override;
    QPainterPath build_path() override;


private:
    mutable qreal m_width;
    mutable qreal m_height;

    mutable std::string m_width_exp;
    mutable std::string m_height_exp;
};

class ShapeTriangleFacotry final : public ShapeFactoryBase
{
public:
    ShapeTriangleFacotry();
    ~ShapeTriangleFacotry() override;

    ShapeBase *create_shape(const QString& shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const override;
};
}

#endif // SHAPETRIANGLE_H
