#include "shapeRectangle.h"
#include "shapeDefines.h"
#include <PointE.h>

namespace pm
{
class ParamMgr;
}

namespace sp
{


ShapeRectangle::ShapeRectangle(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> & params, ShapePointGraphicsItem* parent_attach_point)
    : ShapeBase(param_mgr, params, parent_attach_point)
    , m_width(SHAPE_RECTANGLE_WIDTH_DEFAULT_VALUE)
    , m_height(SHAPE_RECTANGLE_HEIGHT_DEFAULT_VALUE)
    , m_width_exp("50.0")
    , m_height_exp("50.0")
{
    for(auto idx = 0; idx < 4; idx++)
    {
        m_point_items.push_back(new ShapePointGraphicsItem(QPointF(0, 0), this->shape_graphics_item(), idx));
    }
}

ShapeRectangle::~ShapeRectangle()
{

}

void ShapeRectangle::update_variables()
{
    // 更新shape_item
    ShapeBase::update_variables();

    for(auto i = m_params.begin(); i != m_params.end(); ++i)
    {
        pm::ParamDecl param = *i;
        if(param.key() == SHAPE_RECTANGLE_WIDTH)
        {
            m_width = param.to_float(mp_param_mgr);
            m_width_exp = param.to_str().toStdString();
        }
        else if(param.key() == SHAPE_RECTANGLE_HEIGHT)
        {
            m_height = param.to_float(mp_param_mgr);
            m_height_exp = param.to_str().toStdString();
        }
    }
}

void ShapeRectangle::update_attach_pointes()
{
    // 按照leftTop,leftDown,rightDown,rightTop的顺序进行排列
    m_attach_points_e =
    {
        pm::PointE("0", m_height_exp),
        pm::PointE("0", "0"),
        pm::PointE(m_width_exp, "0"),
        pm::PointE(m_width_exp, m_height_exp),
    };
}

QPainterPath ShapeRectangle::build_path()
{
    // 定义矩形的左下角和右上角点
    QPointF bottom_left(0, 0);
    QPointF top_right(m_width, m_height);

    // 创建矩形路径
    QPainterPath path;
    path.addRect(QRectF(bottom_left, top_right));
    return path;
}

ShapeRectangleFacotry::ShapeRectangleFacotry()
    : ShapeFactoryBase()
{

}

ShapeRectangleFacotry::~ShapeRectangleFacotry()
{

}

ShapeBase *ShapeRectangleFacotry::create_shape(const QString &shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const
{
    return new ShapeRectangle(param_mgr, params, parent_attach_point);
}
}
