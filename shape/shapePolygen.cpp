#include "shapePolygen.h"
#include <PointE.h>
#include <qregularexpression.h>
#include "paramUtils.h"

using namespace sp;

ShapePolygen::ShapePolygen(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> & params, ShapePointGraphicsItem* parent_attach_point)
    : ShapeBase(param_mgr, params, parent_attach_point)
{
    auto params_expand = pm::ParamUtils::expand(params);  // HINT@leixunyong。此处似乎是没有必要展开的，因为已经在创建的时候就展开了。
    remove_no_coord_params(params_expand);
    Q_ASSERT(params_expand.size() % 2 == 0);
    int coord_number = params_expand.size() / 2;
    for(auto idx = 0; idx < coord_number; idx++)
    {
        m_point_items.push_back(new ShapePointGraphicsItem(QPointF(0, 0), this->shape_graphics_item(), idx));
    }
}

ShapePolygen::~ShapePolygen()
{

}

void ShapePolygen::update_variables()
{
    // 更新shape_item
    ShapeBase::update_variables();

    //展开表达式 只包含全局参数
    auto params_expand = pm::ParamUtils::expand(m_params);  // HINT@leixunyong。此处似乎是没有必要展开的，因为已经在创建的时候就展开了。
    remove_no_coord_params(params_expand);
    Q_ASSERT(params_expand.size() % 2 == 0);

    QVector<QPointF> points_pos;
    for(auto idx = 0; idx < params_expand.size() - 1; idx += 2)
    {
        qreal x_pos = params_expand[idx].to_double(mp_param_mgr);
        qreal y_pos = params_expand[idx + 1].to_double(mp_param_mgr);
        points_pos.push_back(QPointF(x_pos, y_pos));
    }
    m_points = points_pos;
}

void ShapePolygen::update_attach_pointes()
{
    // HINT@leixunyong。没有考虑旋转
    auto params = m_params;
    remove_no_coord_params(params);
    QVector<pm::PointE> points_e;
    for(auto idx = 0; idx < params.size() - 1; idx += 2)
    {
        points_e.push_back(pm::PointE(params[idx].expression(), params[idx + 1].expression()));
    }
    qSwap( points_e, m_attach_points_e);
}

QPainterPath ShapePolygen::build_path()
{
    QPainterPath path;
    QVector<QPointF> points = m_points;
    points.insert(points.begin(), QPointF(0, 0));
    if (!points.isEmpty())
    {
        path.moveTo(points[0]);
        for (int i = 1; i < points.size(); ++i)
        {
            path.lineTo(points[i]);
        }
        path.closeSubpath(); // 关闭路径，形成多边形
    }

    return path;

}

void ShapePolygen::remove_no_coord_params(QVector<pm::ParamDecl> &params)
{
    bool continue_check = true;
    while(continue_check)  // 移除和坐标无关的参数
    {
        continue_check = false;
        for(auto idx = 0 ; idx < params.size(); idx++)
        {
            const auto& param_name = params[idx].key();
            // 定义正则表达式，匹配 "x" 后跟一个或多个数字
            QRegularExpression regex("^x\\d+$");
            QRegularExpressionMatch match_x = regex.match(param_name);

            // 定义正则表达式，匹配 "y" 后跟一个或多个数字
            QRegularExpression regey("^y\\d+$");
            QRegularExpressionMatch match_y = regey.match(param_name);

            if(!match_x.hasMatch() && !match_y.hasMatch())
            {
                continue_check = true;
                params.remove(idx);
                break;
            }
        }

    }
}

ShapePolygenFacotry::ShapePolygenFacotry()
{

}

ShapePolygenFacotry::~ShapePolygenFacotry()
{

}

ShapeBase *ShapePolygenFacotry::create_shape(const QString &shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const
{
    return new ShapePolygen(param_mgr, params, parent_attach_point);
}


