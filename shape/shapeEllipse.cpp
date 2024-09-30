#include "shapeEllipse.h"
#include "shapeDefines.h"
#include <PointE.h>
#include <QtMath>
#include <ShapeUtils.h>

namespace pm
{
class ParamMgr;
}

namespace sp
{
ShapeEllipse::ShapeEllipse(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> & params, ShapePointGraphicsItem* parent_attach_point)
    : ShapeBase(param_mgr, params, parent_attach_point)
    , m_semi_major_axis_length(SHAPE_ELLIPSE_SEMI_MAJOR_AXIS_DEFAULT_VALUE)
    , m_semi_minor_axis_length(SHAPE_ELLIPSE_SEMI_MINOR_AXIS_DEFAULT_VALUE)
    , m_start_angle(SHAPE_ELLIPSE_START_ANGLE_DEFAULT_VALUE)
    , m_end_angle(SHAPE_ELLIPSE_END_ANGLE_DEFAULT_VALUE)
    , m_semi_major_axis_length_exp("50")
    , m_semi_minor_axis_length_exp("25")
{
    for(auto idx = 0; idx < 4; idx++)
    {
        m_point_items.push_back(new ShapePointGraphicsItem(QPointF(0, 0), this->shape_graphics_item(), idx));
    }
}

ShapeEllipse::~ShapeEllipse()
{

}

void ShapeEllipse::update_variables()
{
    // 更新shape_item
    ShapeBase::update_variables();

    for (auto i = m_params.begin(); i != m_params.end(); ++i)
    {
        pm::ParamDecl param = * i;
        if (param.key() == SHAPE_ELLIPSE_SEMI_MAJOR_AXIS)
        {
            m_semi_major_axis_length = param.to_float(mp_param_mgr);
            m_semi_major_axis_length_exp = param.to_str().toStdString();
        }
        else if (param.key() == SHAPE_ELLIPSE_SEMI_MINOR_AXIS)
        {
            m_semi_minor_axis_length = param.to_float(mp_param_mgr);
            m_semi_minor_axis_length_exp = param.to_str().toStdString();
        }
        else if (param.key() == SHAPE_ELLIPSE_START_ANGLE)
        {
            m_start_angle = param.to_float(mp_param_mgr);
        }
        else if (param.key() == SHAPE_ELLIPSE_END_ANGLE)
        {
            m_end_angle = param.to_float(mp_param_mgr);
        }
    }

}

void ShapeEllipse::update_attach_pointes()
{
    // 按照leftTop,leftDown,rightDown,rightTop的顺序进行排列
    m_attach_points_e =
    {
        pm::PointE(-pm::Expression(m_semi_major_axis_length_exp.c_str()), m_semi_minor_axis_length_exp),
        pm::PointE(-pm::Expression(m_semi_major_axis_length_exp.c_str()), -pm::Expression(m_semi_minor_axis_length_exp.c_str())),
        pm::PointE(m_semi_major_axis_length_exp, -pm::Expression(m_semi_minor_axis_length_exp.c_str())),
        pm::PointE(m_semi_major_axis_length_exp, m_semi_minor_axis_length_exp),
    };
}

QPainterPath ShapeEllipse::build_path()
{
    // 定义椭圆的参数
    QPointF center(0, 0);
    qreal semiMajorAxis = m_semi_major_axis_length;
    qreal semiMinorAxis = m_semi_minor_axis_length;
    qreal startAngle = -m_start_angle;  // HINT@leixunyong, 为了让效果正常，此处不得不反向。下同。
    qreal endAngle = -m_end_angle;

    // 计算起始点和终止点
    QPointF startPoint(semiMajorAxis * qCos(qDegreesToRadians(m_start_angle)),
                       semiMinorAxis * qSin(qDegreesToRadians(m_start_angle)));
    QPointF endPoint(semiMajorAxis * qCos(qDegreesToRadians(m_end_angle)),
                     semiMinorAxis * qSin(qDegreesToRadians(m_end_angle)));

    // 创建椭圆弧路径
    QPainterPath path;
    if(!ShapeUtils::is_equal(startPoint, endPoint))
    {
        QRectF ellipseRect(-semiMajorAxis, -semiMinorAxis, 2 * semiMajorAxis, 2 * semiMinorAxis);
        path.arcMoveTo(ellipseRect, startAngle);
        path.arcTo(ellipseRect, startAngle, endAngle - startAngle);
        path.lineTo(center);          // 绘制从终止点到中心点的连线
        path.lineTo(startPoint);      // 绘制从中心点到起点的连线
        path.closeSubpath(); // 闭合路径
    }
    else
    {
        path.addEllipse(QPointF(0, 0), m_semi_major_axis_length, m_semi_minor_axis_length);
    }

    return path;
}

ShapeEllipseFacotry::ShapeEllipseFacotry()
    : ShapeFactoryBase()
{

}

ShapeEllipseFacotry::~ShapeEllipseFacotry()
{

}

ShapeBase *ShapeEllipseFacotry::create_shape(const QString &shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const
{
    return new ShapeEllipse(param_mgr, params, parent_attach_point);
}
}
