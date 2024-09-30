#include "shapeSector.h"
#include "shapeDefines.h"
#include "pointE.h"
#include <qmath.h>

namespace pm
{
class ParamMgr;
}

namespace sp
{
ShapeSector::ShapeSector(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> & params, ShapePointGraphicsItem* parent_attach_point)
    : ShapeBase(param_mgr, params, parent_attach_point)
    , m_inside_radius(SHAPE_SECTOR_INSIDE_RADIUS_DEFAULT_VALUE)
    , m_outside_radius(SHAPE_SECTOR_OUTSIDE_RADIUS_DEFAULT_VALUE)
    , m_start_angle(SHAPE_SECTOR_START_ANGLE_DEFAULT_VALUE)
    , m_end_angle(SHAPE_SECTOR_END_ANGLE_DEFAULT_VALUE)
    , m_outside_radius_exp(std::to_string(SHAPE_SECTOR_OUTSIDE_RADIUS_DEFAULT_VALUE))
{
    for(auto idx = 0; idx < 4; idx++)
    {
        m_point_items.push_back(new ShapePointGraphicsItem(QPointF(0, 0), this->shape_graphics_item(), idx));
    }
}

ShapeSector::~ShapeSector()
{

}

void ShapeSector::update_variables()
{
    ShapeBase::update_variables();

    for (auto i = m_params.begin(); i != m_params.end(); ++i)
    {
        pm::ParamDecl param = * i;
        if (param.key() == SHAPE_SECTOR_INSIDE_RADIUS)
        {
            m_inside_radius = param.to_float(mp_param_mgr);
        }
        else if (param.key() == SHAPE_SECTOR_OUTSIDE_RADIUS)
        {
            m_outside_radius = param.to_float(mp_param_mgr);
            m_outside_radius_exp = param.to_str().toStdString();
        }
        else if (param.key() == SHAPE_SECTOR_START_ANGLE)
        {
            m_start_angle = param.to_float(mp_param_mgr);
        }
        else if (param.key() == SHAPE_SECTOR_END_ANGLE)
        {
            m_end_angle = param.to_float(mp_param_mgr);
        }
    }
}

void ShapeSector::update_attach_pointes()
{
    m_attach_points_e =
    {
        pm::PointE(-pm::Expression(m_outside_radius_exp.c_str()), m_outside_radius_exp),
        pm::PointE(-pm::Expression(m_outside_radius_exp.c_str()), -pm::Expression(m_outside_radius_exp.c_str())),
        pm::PointE(m_outside_radius_exp, -pm::Expression(m_outside_radius_exp.c_str())),
        pm::PointE(m_outside_radius_exp, m_outside_radius_exp),
    };
}

QPainterPath ShapeSector::build_path()
{
    QPainterPath sector_path;

    qreal semiMajorAxis = m_outside_radius;
    qreal semiMinorAxis = m_outside_radius;
    qreal startAngle = -m_start_angle;  // HINT@leixunyong, 为了让效果正常，此处不得不反向。下同。
    qreal endAngle = -m_end_angle;

    // 计算起始点和终止点
    QPointF out_start_point(semiMajorAxis* qCos(qDegreesToRadians(m_start_angle)),
                            semiMinorAxis* qSin(qDegreesToRadians(m_start_angle)));
    QPointF out_end_point(semiMajorAxis* qCos(qDegreesToRadians(m_end_angle)),
                          semiMinorAxis* qSin(qDegreesToRadians(m_end_angle)));

    // 创建外圈圆弧路径
    QRectF out_ellipse_rect(-semiMajorAxis, -semiMinorAxis, 2 * semiMajorAxis, 2 * semiMinorAxis);
    // 创建内圈圆弧路径
    semiMajorAxis = m_inside_radius;
    semiMinorAxis = m_inside_radius;

    // 计算起始点和终止点
    QPointF in_start_point(semiMajorAxis* qCos(qDegreesToRadians(m_start_angle)),
                           semiMinorAxis* qSin(qDegreesToRadians(m_start_angle)));
    QPointF in_end_point(semiMajorAxis* qCos(qDegreesToRadians(m_end_angle)),
                         semiMinorAxis* qSin(qDegreesToRadians(m_end_angle)));
    // 创建内圈圆弧路径
    QRectF in_ellipse_rect(-semiMajorAxis, -semiMinorAxis, 2 * semiMajorAxis, 2 * semiMinorAxis);
    sector_path.arcMoveTo(out_ellipse_rect, startAngle);
    sector_path.arcTo(out_ellipse_rect, startAngle, endAngle - startAngle); // 从外圈起点到外圈终点
    sector_path.lineTo(in_end_point);                                       // 从外圈终点到内圈终点
    sector_path.arcTo(in_ellipse_rect, endAngle, -(endAngle - startAngle)); // 从内圈终点到内圈起点
    sector_path.lineTo(out_start_point);                                    // 从内圈起点到外圈起点
    // circle_path.arcMoveTo(in_ellipse_rect, startAngle);
    sector_path.closeSubpath(); // 闭合路径
    return sector_path;
}

ShapSectoreFacotry::ShapSectoreFacotry()
    : ShapeFactoryBase()
{

}

ShapSectoreFacotry::~ShapSectoreFacotry()
{

}

ShapeBase *ShapSectoreFacotry::create_shape(const QString &shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const
{
    return new ShapeSector(param_mgr, params, parent_attach_point);
}

}
