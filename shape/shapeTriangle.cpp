#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <qboxlayout.h>
#include "shapeTriangle.h"
#include "shapeDefines.h"
#include "pointE.h"

namespace pm
{
class ParamMgr;
}

namespace sp
{
ShapeTriangle::ShapeTriangle(pm::ParamMgr *param_mgr,  const QVector<pm::ParamDecl> & params, ShapePointGraphicsItem* parent_attach_point)
    : ShapeBase(param_mgr, params, parent_attach_point)
    , m_width(SHAPE_TRIANGLE_WIDTH_DEFAULT_VALUE)
    , m_height(SHAPE_TRIANGLE_HEIGHT_DEFAULT_VALUE)
    , m_width_exp("50")
    , m_height_exp("50")
{
    for(auto idx = 0; idx < 4; idx++)
    {
        m_point_items.push_back(new ShapePointGraphicsItem(QPointF(0, 0), this->shape_graphics_item(), idx));
    }
}

ShapeTriangle::~ShapeTriangle()
{

}

void ShapeTriangle::update_variables()
{
    ShapeBase::update_variables();

    for(auto i = m_params.begin(); i != m_params.end(); ++i)
    {
        pm::ParamDecl param = *i;
        if(param.key() == SHAPE_TRIANGLE_WIDTH)
        {
            m_width = param.to_float(mp_param_mgr);
            m_width_exp = param.to_str().toStdString();
        }
        else if(param.key() == SHAPE_TRIANGLE_HEIGHT)
        {
            m_height = param.to_float(mp_param_mgr);
            m_height_exp = param.to_str().toStdString();
        }
    }
}

void ShapeTriangle::update_attach_pointes()
{
    // 按照leftTop,leftDown,rightDown,rightTop的顺序进行排列
    m_attach_points_e = QVector<pm::PointE>
    {
        pm::PointE("0", m_height_exp),
        pm::PointE("0", "0"),
        pm::PointE(m_width_exp, "0"),
        pm::PointE(m_width_exp, m_height_exp),
    };
}

QPainterPath ShapeTriangle::build_path()
{
    // 绘制三角形
    QPointF p1(0, 0);
    QPointF p2(m_width, 0);
    QPointF p3(0, m_height);

    // 创建三角形路径
    QPainterPath path;
    path.moveTo(p1);
    path.lineTo(p2);
    path.lineTo(p3);
    path.closeSubpath();

    return path;
}

ShapeTriangleFactory::ShapeTriangleFactory()
    : ShapeFactoryBase()
{

}

ShapeTriangleFactory::~ShapeTriangleFactory()
{

}

ShapeBase *ShapeTriangleFactory::create_shape(const QString &shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const
{
    return new ShapeTriangle(param_mgr, params, parent_attach_point);
}

}
