#include "priUtils.h"
#include "Primitive.h"
#include "attachTreeNodeMgr.h"
#include "shapeDefines.h"

#include <ShapeDrawGraphicsItem.h>
#include <priTreeWidgetItem.h>
#include <viewport.h>
#include "shapeBase.h"
#include "qpolygon.h"
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
            if(tree_node->shape()->shape_type() == SHAPE_POLYGEN)
            {
                auto parent_attach_point = at::AttachTreeUtils::parent_attach_point_global_pos(tree_node);
                polygon << parent_attach_point;
            }
            for(auto idx = 0; idx < attach_points.size(); idx++)
            {
                auto attach_point = at::AttachTreeUtils::attach_point(tree_node, idx, true);
                polygon << attach_point;
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
