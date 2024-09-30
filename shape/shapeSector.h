#ifndef SECTORSHAPE_H
#define SECTORSHAPE_H

#include "shapeBase.h"
#include "shapeFactoryBase.h"

namespace pm
{
class ParamMgr;
class PointE;
}

namespace sp
{
class SP_PUBLIC ShapeSector final: public ShapeBase
{
public:
    ShapeSector(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> & params, ShapePointGraphicsItem* parent_attach_point);
    ~ShapeSector() override;

protected:
    void update_variables() override;
    void update_attach_pointes() override;
    QPainterPath build_path() override;

private:
    mutable qreal m_inside_radius;
    mutable qreal m_outside_radius;
    mutable qreal m_start_angle;
    mutable qreal m_end_angle;

    mutable std::string m_outside_radius_exp;
};

class ShapSectoreFacotry final : public ShapeFactoryBase
{
public:
    ShapSectoreFacotry();
    ~ShapSectoreFacotry() override;

    ShapeBase *create_shape(const QString &shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const override;
};

}

#endif // SECTORSHAPE_H
