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

    static void classify_graphics_items(
        Primitive* pri,
        const QPointF& click_pos,
        const QList<QGraphicsItem*>& graphics_item,
        sp::ShapeDrawGraphicsItem*& shape_item,
        sp::ShapePointGraphicsItem*& point_item
    );
};
}


#endif // PRIUTILS_H
