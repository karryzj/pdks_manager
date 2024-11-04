#include "shapeCurve.h"
#include "common_Defines.h"
#include "configManager.h"
#include "shapeDefines.h"
#include "shapePointGraphicsItem.h"
#include "shapeUtils.h"

#include "ParamUtils.h"

#include <PointE.h>
namespace pm
{
class ParamMgr;
}

namespace sp
{
ShapeCurve::ShapeCurve(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl>& params, ShapePointGraphicsItem* parent_attach_point)
    : ShapeBase(param_mgr, params, parent_attach_point)
{
}

ShapeCurve::~ShapeCurve()
{

}

const QVector<QPointF> &sp::ShapeCurve::point_set() const
{
    return m_point_set;
}

void ShapeCurve::update_attach_pointes()
{
    m_attach_points_e = {};
}

void ShapeCurve::update_variables()
{
    // 更新shape_item
    ShapeBase::update_variables();

    QVector<QPair<QString, QString>> point_info;
    // 更新自己
    for (auto i = m_params.begin(); i != m_params.end(); ++i)
    {
        pm::ParamDecl param = * i;
        const QString& param_key = param.key();
        if(param_key.contains(SHAPE_CURVE_EQUATION) || param_key.contains(SHAPE_CURVE_FIXED_POINT_SET))
        {
            const QString& param_str = param.expression();
            QPair<QString, QString> pair(param_key, param_str);
            point_info.append(pair);
        }
    }

    m_point_set.clear();
    for(auto idx = 0; idx < point_info.size() - 1; idx += 2)
    {
        const auto& key = point_info[idx].first;
        const auto& value_reverse = point_info[idx + 1].second;
        bool reverse = false;
        if(value_reverse == SHAPE_CURVE_REVERSE_TURE)
        {
            reverse = true;
        }
        else if(value_reverse == SHAPE_CURVE_REVERSE_FALSE)
        {
            reverse = false;
        }
        else
        {
            Q_ASSERT(false);
        }

        if(key.contains(SHAPE_CURVE_EQUATION))
        {
            analyse_equation(point_info[idx].second, m_point_set, reverse);
        }
        else if(key.contains(SHAPE_CURVE_FIXED_POINT_SET))
        {
            analyse_fixed_point_set(point_info[idx].second, m_point_set, reverse);
        }
    }
}

QPainterPath ShapeCurve::build_path()
{
    // 定义圆心和半径
    QPainterPath path;
    if (!m_point_set.isEmpty())
    {
        path.moveTo(m_point_set[0]);
        for (int i = 1; i < m_point_set.size(); ++i)
        {
            path.lineTo(m_point_set[i]);
        }
        path.closeSubpath();  // 关闭路径，形成多边形
    }
    return path;
}

void ShapeCurve::analyse_equation(const QString &equation, QVector<QPointF> &point_set, bool reverse)
{
    QVector<QString> sub_strs = ShapeUtils::split_string(equation, "\n");
    QVector<QPointF> new_fix_point_set;
    QVector<pm::ParamDecl> params;
    for(const auto& sub_str : sub_strs)
    {
        pm::ParamDecl param_constraint("", sub_str);
        params.push_back(param_constraint);
    }
    auto min_dist_threshold = cm::ConfigManager::instance()->query(CM_ARC_LEN_KEY).toDouble();
    auto points = pm::ParamUtils::gen_curve_points(mp_param_mgr, params, min_dist_threshold);
    if(reverse)
    {
        std::reverse(points.begin(), points.end());
    }

    point_set.append(points);
}

void ShapeCurve::analyse_fixed_point_set(const QString &fixed_point_set, QVector<QPointF> &point_set, bool reverse)
{
    QVector<QString> sub_strs = ShapeUtils::split_string(fixed_point_set, "\n");
    QVector<QPointF> new_fix_point_set;
    for(const auto& sub_str : sub_strs)
    {
        QVector<QString> coordinates = ShapeUtils::split_string(sub_str, ",");
        Q_ASSERT(coordinates.size() == 2);

        const auto& x_coord_str = coordinates[0];
        const auto& y_coord_str = coordinates[1];

        qreal x_coord = mp_param_mgr->parse(x_coord_str);
        qreal y_coord = mp_param_mgr->parse(y_coord_str);
        QPointF point(x_coord, y_coord);
        reverse ? new_fix_point_set.prepend(point) : new_fix_point_set.append(point);
    }

    point_set.append(new_fix_point_set);
}

ShapeCurveFactory::ShapeCurveFactory()
    : ShapeFactoryBase()
{

}

ShapeCurveFactory::~ShapeCurveFactory()
{

}

ShapeBase *ShapeCurveFactory::create_shape(const QString &shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const
{
    return new ShapeCurve(param_mgr, params, parent_attach_point);
}

}
