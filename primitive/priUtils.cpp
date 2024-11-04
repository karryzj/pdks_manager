#include "priUtils.h"
#include "Primitive.h"
#include "attachTreeNodeMgr.h"
#include "shapeDefines.h"

#include <ShapeCurve.h>
#include <priTreeWidgetItem.h>
#include <viewport.h>
#include <viewport.h>
#include "shapeUtils.h"
#include "qpolygon.h"

#include "common_Defines.h"
#include "configManager.h"

#include "ParamUtils.h"
using namespace pr;

bool PriUtils::is_x_coord_align(const QPointF &p_1, const QPointF &p_2)
{
    return abs(p_1.x() - p_2.x()) < SHAPE_COORD_PRECISION ? true : false;
}

bool PriUtils::is_y_coord_align(const QPointF &p_1, const QPointF &p_2)
{
    return abs(p_1.y() - p_2.y()) < SHAPE_COORD_PRECISION ? true : false;
}

QVector<pm::ParamDecl> pr::PriUtils::organize_and_eliminate_params(const QVector<pm::ParamDecl> &param_list, int node_id)
{
    QVector<pm::ParamDecl> result_params;

    int idx = 0;
    int idy = 1;
    while(idx < param_list.size() && idy < param_list.size())
    {
        const auto& param_x = param_list[idx];
        const auto& param_x_name = param_x.key();
        double param_x_value = param_x.value();

        QString result_param_x_name = param_x_name;
        QString result_param_x_expression = "N" + QString::number(node_id) + "_" + "d" + param_x.key();
        double result_param_x_value = param_x_value;
        if(idx > 1)
        {
            result_param_x_expression = result_params[idx - 2].expression() + "+" + result_param_x_expression;
        }

        // 检查是否和前面的值一样
        for(auto idx_front = 0; idx_front < idx; idx_front += 2)
        {
            const auto& front_param_x = result_params[idx_front];
            if(front_param_x.value() != result_param_x_value)
            {
                continue;
            }
            result_param_x_expression = front_param_x.expression();
            break;
        }
        if(is_x_coord_align(QPointF(0, 0), QPointF(result_param_x_value, 0) ))
        {
            result_param_x_expression = "0";
            result_param_x_value = 0;
        }
        if(result_param_x_expression.size() >= 2 && result_param_x_expression.startsWith("0+"))
        {
            result_param_x_expression.remove(0, 2);
        }

        pm::ParamDecl result_param_x;
        result_param_x.set_key(result_param_x_name);
        result_param_x.set_expression(result_param_x_expression);
        result_param_x.set_value(param_x_value);
        result_params.push_back(result_param_x);

        const auto& param_y = param_list[idy];
        const auto& param_y_name = param_y.key();
        double param_y_value = param_y.value();

        QString result_param_y_name = param_y_name;
        QString result_param_y_expression = "N" + QString::number(node_id) + "_" + "d" + param_y.key();
        double result_param_y_value = param_y_value;
        if(idy > 2)
        {
            result_param_y_expression = result_params[idy - 2].expression() + "+" + result_param_y_expression;
        }

        // 和前面每个点都做对比
        for(auto idy_front = 1; idy_front < idy; idy_front += 2)
        {
            const auto& front_param_y = result_params[idy_front];
            if(front_param_y.value() != result_param_y_value)
            {
                continue;
            }

            result_param_y_expression = front_param_y.expression();
            break;
        }
        // 和原点做对比
        if(is_y_coord_align(QPointF(0, 0), QPointF(0, result_param_y_value) ))
        {
            result_param_y_expression = "0";
            result_param_y_value = 0;
        }
        if(result_param_y_expression.size() >= 2 && result_param_y_expression.startsWith("0+"))
        {
            result_param_y_expression.remove(0, 2);
        }
        pm::ParamDecl result_param_y;
        result_param_y.set_key(result_param_y_name);
        result_param_y.set_expression(result_param_y_expression);
        result_param_y.set_value(param_y_value);
        result_params.push_back(result_param_y);

        idx += 2;
        idy += 2;
    }

    return result_params;
}

void pr::PriUtils::setup_polygon_extracted_param(int node_id, qreal default_value, pm::ParamDecl &param)
{
    const QString& param_key =  param.key(); // 示例输入字符串

    // 使用正则表达式解析字符串
    QRegularExpression regex("([xy])(\\d+)");
    QRegularExpressionMatch match = regex.match(param_key);

    if (!match.hasMatch())
    {
        return;
    }
    QString character = match.captured(1); // 提取字符 'x' 或 'y'
    QString number_str = match.captured(2);
    int number = number_str.toInt(); // 提取整数

    if(number <= 0)
    {
        return;
    }

    QString node_id_str = QString::number(node_id);


    QString exp = "N" + node_id_str + "d" + param_key;


}

void pr::PriUtils::classify_graphics_items(
    Primitive* pri,
    const QPointF &scene_pos,
    const QList<QGraphicsItem *> &graphics_item,
    sp::ShapeDrawGraphicsItem *&shape_item,
    sp::ShapePointGraphicsItem *&point_item)
{
    QVector<sp::ShapeDrawGraphicsItem*> shape_items;
    QVector<sp::ShapePointGraphicsItem*> point_items;

    auto logic_pos = pri->tree_node_mgr()->viewport()->map_from_scene(scene_pos);

    for(auto item : graphics_item)
    {
        auto shape_item = dynamic_cast<sp::ShapeDrawGraphicsItem*>(item);
        if(shape_item)
        {
            auto tree_node = at::AttachTreeUtils::attach_tree_node_shape_item_in(shape_item, pri->at_root());

            const QVector<QPointF> & attach_points = shape_item->shape_info()->attach_points();
            QPolygonF polygon;
            if(tree_node->shape_name() == SHAPE_POLYGEN)
            {
                auto parent_attach_point = at::AttachTreeUtils::parent_attach_point_global_pos(tree_node);
                polygon << parent_attach_point;
            }

            if(shape_item->shape_name() != SHAPE_CURVE)
            {
                for(auto idx = 0; idx < attach_points.size(); idx++)
                {
                    auto attach_point = at::AttachTreeUtils::attach_point_coord(tree_node, idx, true);
                    polygon << attach_point;
                }
            }
            else
            {
                auto points = dynamic_cast<sp::ShapeCurve*>(shape_item->shape_info())->point_set();
                for(auto point : points)
                {
                    polygon << at::AttachTreeUtils::point_coord(tree_node, point, true);
                }
            }

            if(polygon.containsPoint(logic_pos, Qt::OddEvenFill))
            {
                shape_items.push_back(shape_item);
            }
        }
        auto point_item = dynamic_cast<sp::ShapePointGraphicsItem*>(item);
        if(point_item)
        {
            if(point_item->boundingRect().contains(scene_pos) && point_item->isVisible())
            {
                point_items.push_back(point_item);
            }
            else
            {
                point_item->setVisible(false);
            }
        }
    }

    if(shape_items.size() > 0)
    {
        auto area_path = [](const QPainterPath& path)
        {
            QPolygonF polygon = path.toFillPolygon();
            // 计算多边形的面积
            double area = 0.0;
            int n = polygon.size();
            for (int i = 0; i < n; ++i)
            {
                QPointF p1 = polygon[i];
                QPointF p2 = polygon[(i + 1) % n];
                area += p1.x() * p2.y() - p2.x() * p1.y();
            }
            area = std::abs(area) / 2.0;
            return area;
        };

        QMap<qreal, sp::ShapeDrawGraphicsItem*> area_to_draw_item;
        for(auto idx = 0; idx < shape_items.size(); idx++)
        {
            auto area = area_path(shape_items[idx]->shape_info()->painter_path());
            area_to_draw_item.insert(area, shape_items[idx]); // HINT@leixunyong。这里其实不用担心key重复的问题。
        }

        shape_item = *area_to_draw_item.begin();

        for(auto item : graphics_item)
        {
            auto sp_item = dynamic_cast<sp::ShapeDrawGraphicsItem*>(item);
            if(sp_item != nullptr && sp_item != shape_item)
            {
                sp_item->set_point_items_visible(false);
            }
        }
    }
    else
    {
        shape_item = nullptr;
    }


    if(point_items.size() > 0)
    {
        point_item = point_items[0];
    }
    else
    {
        point_item = nullptr;
    }

    return;
}

QPointF pr::PriUtils::closest_grid_point(const QPointF &point,  at::AttachTreeBaseNode* node, bool is_logic_pos)
{
    QPointF logic_pos = point;
    if(!is_logic_pos)
    {
        logic_pos = node->tree_node_mgr()->viewport()->map_from_scene(point);
    }

    qreal res = cm::ConfigManager::instance()->query(CM_LOGIC_GRID_RESOLUTION_KEY).toDouble();

    qreal adjusted_x = qRound(logic_pos.x() / res) * res;
    qreal adjusted_y = qRound(logic_pos.y() / res) * res;
    QPointF new_pos(adjusted_x, adjusted_y);
    return new_pos;
}

void pr::PriUtils::modify_preview_polygen_by_points(const QVector<QPair<QPointF, sp::ShapePointGraphicsItem *> > &chosen_coord_points, Primitive* pri, QGraphicsPolygonItem &preview_polygen)
{
    if(chosen_coord_points.size() < 2)
    {
        preview_polygen.setVisible(false);
        preview_polygen.update();
    }

    // 第一个点是父附着点
    QPolygonF polygen;
    auto root_point_item = sp::ShapePointGraphicsItem::cachedItem;
    if(nullptr == root_point_item)
    {
        auto origin_point_pos = pri->tree_node_mgr()->viewport()->map_to_scene(QPointF(0, 0));
        polygen << origin_point_pos;
    }
    else
    {
        polygen << root_point_item->pos();
    }

    for(auto pair : chosen_coord_points)
    {
        polygen << pair.second->pos();
    }

    preview_polygen.setPolygon(polygen);
    preview_polygen.setVisible(true);
    preview_polygen.update();
}

bool PriUtils::is_curve_equation_valid(const QString &str, pm::ParamMgr *param_mgr)
{
    // 先拆分成单个的点坐标
    auto equation_strs = sp::ShapeUtils::split_string(str, "\n");
    if(equation_strs.empty())
    {
        return false;
    }

    QVector<pm::ParamDecl> equation_params;
    for(const auto& equation : equation_strs)
    {
        pm::ParamDecl param("", equation);
        equation_params.append(param);
    }
    auto min_dist_threshold = cm::ConfigManager::instance()->query(CM_ARC_LEN_KEY).toDouble();
    QVector<QPointF> equation_points = pm::ParamUtils::gen_curve_points(param_mgr, equation_params, min_dist_threshold);
    if(equation_points.empty())
    {
        return false;
    }

    return true;
}

bool PriUtils::is_curve_fixed_point_set_valid(const QString &str, pm::ParamMgr *param_mgr)
{
    // 先拆分成单个的点坐标
    auto point_coord_strs = sp::ShapeUtils::split_string(str, "\n");
    if(point_coord_strs.empty())
    {
        return false;
    }

    for(const auto& point_coord : point_coord_strs)
    {
        bool is_point_coord = sp::ShapeUtils::is_point_coord(point_coord, param_mgr);
        if(!is_point_coord)
        {
            return false;
        }
    }
    return true;
}
