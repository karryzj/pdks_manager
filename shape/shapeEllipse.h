#ifndef SHAPEELLIPSE_H
#define SHAPEELLIPSE_H

#include "shapeBase.h"
#include "shapeFactoryBase.h"

namespace sp
{

class SP_PUBLIC ShapeEllipse final: public ShapeBase
{
public:
    ShapeEllipse(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> & params, ShapePointGraphicsItem* parent_attach_point);
    ~ShapeEllipse() override;

protected:
    void update_variables() override;
    void update_attach_pointes() override;
    QPainterPath build_path() override;

private:
    mutable qreal m_semi_major_axis_length;
    mutable qreal m_semi_minor_axis_length;
    mutable qreal m_start_angle;   // 角度制
    mutable qreal m_end_angle;     // 角度制

    mutable std::string m_semi_major_axis_length_exp;
    mutable std::string m_semi_minor_axis_length_exp;
};

// rec shape factory only create RectShape
class ShapeEllipseFactory : public ShapeFactoryBase
{
public:
    ShapeEllipseFactory();
    ~ShapeEllipseFactory() override;

    ShapeBase *create_shape(const QString &shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const override;
};

}

#endif // SHAPEELLIPSE_H
