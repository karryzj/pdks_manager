#include "shapeCircle.h"
#include "shapeDefines.h"
#include "shapePointGraphicsItem.h"

#include <PointE.h>
namespace pm
{
class ParamMgr;
}

namespace sp
{
ShapeCircle::ShapeCircle(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl>& params, ShapePointGraphicsItem* parent_attach_point)
    : ShapeBase(param_mgr, params, parent_attach_point)
    , m_radius(SHAPE_CIRCLE_RADIUS_DEFAULT_VALUE)
    , m_radius_exp("25.0")
{
    for(auto idx = 0; idx < 4; idx++)
    {
        m_point_items.push_back(new ShapePointGraphicsItem(QPointF(0, 0), this->shape_graphics_item(), idx));
    }
}

ShapeCircle::~ShapeCircle()
{

}

void ShapeCircle::update_attach_pointes()
{
    m_attach_points_e =
    {
        pm::PointE(-pm::Expression(m_radius_exp.c_str()), m_radius_exp),
        pm::PointE(-pm::Expression(m_radius_exp.c_str()), -pm::Expression(m_radius_exp.c_str())),
        pm::PointE(m_radius_exp, -pm::Expression(m_radius_exp.c_str())),
        pm::PointE(m_radius_exp, m_radius_exp),
    };
}

void ShapeCircle::update_variables()
{
    // 更新shape_item
    ShapeBase::update_variables();

    // 更新自己
    for (auto i = m_params.begin(); i != m_params.end(); ++i)
    {
        pm::ParamDecl param = * i;
        if (param.key() == SHAPE_CIRCLE_RADIUS)
        {
            m_radius = param.to_float(mp_param_mgr);
            m_radius_exp = param.to_str().toStdString();
        }
        else
        {
            Q_ASSERT(false);
        }
    }
}

QPainterPath ShapeCircle::build_path()
{
    // 定义圆心和半径
    QPointF center(0, 0);
    QPainterPath path;
    path.addEllipse(center, m_radius, m_radius);
    return path;
}

ShapeCircleFacotry::ShapeCircleFacotry()
    : ShapeFactoryBase()
{

}

ShapeCircleFacotry::~ShapeCircleFacotry()
{

}

ShapeBase *ShapeCircleFacotry::create_shape(const QString &shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const
{
    return new ShapeCircle(param_mgr, params, parent_attach_point);
}

}