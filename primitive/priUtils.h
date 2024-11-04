#ifndef PRIUTILS_H
#define PRIUTILS_H

// 用于定义一些简单的数据结构
#include "qpoint.h"
#include <PriTreeWidget.h>

class QGraphicsItem;
namespace pr
{
class Primitive;
class PriTreeWidgetItem;
class PriUtils final
{
public:
    PriUtils () = delete;

    static bool is_x_coord_align(const QPointF& p_1, const QPointF& p_2);
    static bool is_y_coord_align(const QPointF& p_1, const QPointF& p_2);

    static QVector<pm::ParamDecl> organize_and_eliminate_params(const QVector<pm::ParamDecl>& origin_params, int node_id);
    static void setup_polygon_extracted_param(int node_id, qreal default_value, pm::ParamDecl& param);

    static void classify_graphics_items(
        Primitive* pri,
        const QPointF& click_pos,
        const QList<QGraphicsItem*>& graphics_item,
        sp::ShapeDrawGraphicsItem*& shape_item,
        sp::ShapePointGraphicsItem*& point_item
    );

    static QPointF closest_grid_point(const QPointF& point, at::AttachTreeBaseNode* node, bool is_logic_pos = false);

    static void modify_preview_polygen_by_points(const QVector<QPair<QPointF, sp::ShapePointGraphicsItem*>>& chosen_coord_points, Primitive* pri, QGraphicsPolygonItem& preview_polygen);

    static bool is_curve_equation_valid(const QString& str, pm::ParamMgr* param_mgr); // TODO@leixunyong。这里还没有一个明确的筛选逻辑，先空实现

    static bool is_curve_fixed_point_set_valid(const QString& str, pm::ParamMgr* param_mgr);
};
}


#endif // PRIUTILS_H
