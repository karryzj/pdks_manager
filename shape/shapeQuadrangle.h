#ifndef QUADRANGLESHAPE_H
#define QUADRANGLESHAPE_H

#include "shapeBase.h"
#include "shapeFactoryBase.h"

namespace pm
{
class ParamMgr;
class PointE;
}

namespace sp
{

class SP_PUBLIC ShapeQuadrangle final: public ShapeBase
{
public:
    ShapeQuadrangle(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> & params, ShapePointGraphicsItem* parent_attach_point);
    ~ShapeQuadrangle() override;

protected:
    void update_variables() override;
    void update_attach_pointes() override;
    QPainterPath build_path() override;

private:
    mutable qreal m_width;
    mutable qreal m_height;
    mutable qreal m_x_1;
    mutable qreal m_x_2;
    mutable qreal m_x_3;
    mutable qreal m_x_4;

    mutable std::string m_width_exp;
    mutable std::string m_height_exp;
};

class ShapeQuadrangleFacotry final : public ShapeFactoryBase
{
public:
    ShapeQuadrangleFacotry();
    ~ShapeQuadrangleFacotry() override;

    ShapeBase *create_shape(const QString &shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const override;
};

}

#endif // QUADRANGLESHAPE_H
