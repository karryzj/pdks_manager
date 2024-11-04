#ifndef PRIMOUSEFOLLOWER_H
#define PRIMOUSEFOLLOWER_H

#include <QGraphicsItem>

class QGraphicsView;
namespace pr
{
class PriMouseFollower final : public QGraphicsItem
{
public:
    PriMouseFollower(QGraphicsView* view);
    ~PriMouseFollower() override;

    void set_corrected_mouse_pos(const QPoint& mouse_pos);
    QPoint global_mouse_pos() const;
    QPointF scene_pos() const;

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override final;

    QRectF boundingRect() const override final;

private:
    QGraphicsView* mp_view;
    QPoint m_corrected_mouse_pos{0, 0};
};

}



#endif // PRIMOUSEFOLLOWER_H
