#ifndef PRIAUXILIARYDRAGBOX_H
#define PRIAUXILIARYDRAGBOX_H

#include <QGraphicsItem>

namespace at
{
class AttachTreeNode;
}

namespace pr
{
class PriAuxiliaryDragBox final: public QGraphicsItem
{
public:
    PriAuxiliaryDragBox(at::AttachTreeNode* tree_node = nullptr);
    ~PriAuxiliaryDragBox() override;

    void set_tree_node(at::AttachTreeNode* tree_node);
    at::AttachTreeNode* tree_node() const;

    bool set_cursor_in_dragging_circle(const QPointF& cursor_pos);

    bool has_started() const;
    void set_has_started(bool has_started);
    void set_started_point(const QPointF& cursor_pos);

    bool is_moveing() const;
    void set_is_moving(bool is_moving);

    void set_current_moving_pos(const QPointF& mouse_pos);

    void use_modification();

    void reset();

private:
    enum class MoveDirection
    {
        RIGHT,
        TOP,
        NONE
    };

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override final;

    QRectF boundingRect() const override final;

private:
    bool direction_is_valid(MoveDirection direction) const;
    QRectF direction_circle_area(MoveDirection direction) const;
    QPointF mid_point_in_direction(MoveDirection direction) const;
    QCursor create_arrow_cursor(qreal angle);
private:
    at::AttachTreeNode* mp_tree_node = nullptr;
    bool m_has_started = false;
    bool m_is_moving = false;

    struct
    {
        MoveDirection move_direction = MoveDirection::NONE;
        QPointF offset{0, 0};
        bool distance_positive = false;
    } m_moving_info;
};
}
#endif // PRIAUXILIARYDRAGBOX_H
