#include "shapeQuadrangle.h"
#include "shapeDefines.h"
#include <PointE.h>

namespace pm
{
class ParamMgr;
}

namespace sp
{
ShapeQuadrangle::ShapeQuadrangle(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> & params, ShapePointGraphicsItem* parent_attach_point)
    : ShapeBase(param_mgr, params, parent_attach_point)
    , m_width(SHAPE_QUADRANGLE_WIDTH_DEFAULT_VALUE)
    , m_height(SHAPE_QUADRANGLE_HEIGHT_DEFAULT_VALUE)
    , m_x_1(SHAPE_QUADRANGLE_ONE_DEFAULT_VALUE)
    , m_x_2(SHAPE_QUADRANGLE_TWO_DEFAULT_VALUE)
    , m_x_3(SHAPE_QUADRANGLE_THREE_DEFAULT_VALUE)
    , m_x_4(SHAPE_QUADRANGLE_FOUR_DEFAULT_VALUE)
    , m_width_exp("50")
    , m_height_exp("50")
{
    for(auto idx = 0; idx < 4; idx++)
    {
        m_point_items.push_back(new ShapePointGraphicsItem(QPointF(0, 0), this->shape_graphics_item(), idx));
    }
}

ShapeQuadrangle::~ShapeQuadrangle()
{

}

void ShapeQuadrangle::update_variables()
{
    // 更新shape_item
    ShapeBase::update_variables();

    for(auto i = m_params.begin(); i != m_params.end(); ++i)
    {
        pm::ParamDecl param = *i;
        if(param.key() == SHAPE_QUADRANGLE_WIDTH)
        {
            m_width = param.to_float(mp_param_mgr);
            m_width_exp = param.to_str().toStdString();
        }
        else if(param.key() == SHAPE_QUADRANGLE_HEIGHT)
        {
            m_height = param.to_float(mp_param_mgr);
            m_height_exp = param.to_str().toStdString();
        }
        else if(param.key() == SHAPE_QUADRANGLE_PARAMETER_1)
        {
            m_x_1 = param.to_float(mp_param_mgr);
        }
        else if(param.key() == SHAPE_QUADRANGLE_PARAMETER_2)
        {
            m_x_2 = param.to_float(mp_param_mgr);
        }
        else if(param.key() == SHAPE_QUADRANGLE_PARAMETER_3)
        {
            m_x_3 = param.to_float(mp_param_mgr);
        }
        else if(param.key() == SHAPE_QUADRANGLE_PARAMETER_4)
        {
            m_x_4 = param.to_float(mp_param_mgr);
        }
    }

}

void ShapeQuadrangle::update_attach_pointes()
{
    // 按照leftTop,leftDown,rightDown,rightTop的顺序进行排列
    QVector<pm::PointE> temp =
    {
        pm::PointE("0", m_height_exp),
        pm::PointE("0", "0"),
        pm::PointE(m_width_exp, "0"),
        pm::PointE(m_width_exp, m_height_exp),
    };

    qSwap(temp, m_attach_points_e);
}

QPainterPath ShapeQuadrangle::build_path()
{
    QPointF p0(0, m_x_1);
    QPointF p1(m_width - m_x_4, 0);
    QPointF p2(m_width, m_height - m_x_3);
    QPointF p3(m_x_2, m_height);

    QPainterPath path;
    path.moveTo(p0);
    path.lineTo(p1);
    path.lineTo(p2);
    path.lineTo(p3);
    path.closeSubpath();
    return path;
}

ShapeQuadrangleFacotry::ShapeQuadrangleFacotry()
    : ShapeFactoryBase()
{

}

ShapeQuadrangleFacotry::~ShapeQuadrangleFacotry()
{

}

ShapeBase *ShapeQuadrangleFacotry::create_shape(const QString &shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const
{
    return new ShapeQuadrangle(param_mgr, params, parent_attach_point);
}

}
