#ifndef POINTITEM_H
#define POINTITEM_H

#include <QGraphicsItem>
#include "spCommon.h"
#include "QObject"
class QGraphicsView;
class QPainter;
namespace at
{
class AttachTreeNode;
}
namespace sp
{
class ShapeDrawGraphicsItem;
class SP_PUBLIC ShapePointGraphicsItem final :  public QObject, public QGraphicsItem
{
    Q_OBJECT;
public:
    ShapePointGraphicsItem(const QPointF& pos, ShapeDrawGraphicsItem* parent, int index, bool is_origin_point = false);
    ~ShapePointGraphicsItem() override;

public:
    void set_new_position(const QPointF& pos);
    const QString& shape_name() const;
    const ShapeDrawGraphicsItem* shape_item() const;
    const QPointF& pos() const;
    QRectF boundingRect() const override;

    bool is_anchor_point() const;
    void set_anchor_point(bool is_anchor_point);

    bool is_coor_anchor_point() const;
    void set_coor_anchor_point(bool is_coor_anchor_point);

private:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    void reset_mouse_move_info();
public:
    static ShapePointGraphicsItem* cachedItem;   // 用来缓存被选中的点Item。在绘制图形的时候会用到。
signals:
    void send_dragged_rectangle_info(ShapePointGraphicsItem* point_item,
                                     QPointF end_point);

private:
    QPointF m_pos;                     // 位置
    QString m_shape_name = "";  // 实例化后的名称
    ShapeDrawGraphicsItem* mp_shape_item = nullptr;
    const int m_index;

    bool m_is_origin_point;

    bool m_is_anchor_point = false;
    bool m_is_coor_anchor_point = false;

    struct MouseMoveInfo
    {
        sp::ShapePointGraphicsItem* point_item = nullptr;
        bool has_been_moved = false;
        QPointF end_point;
        QGraphicsRectItem rectangle_item;
    } m_mouse_move_info;
};

}


#endif // POINTITEM_H
