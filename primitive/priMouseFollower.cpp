#include "priMouseFollower.h"

#include <QGraphicsView>
#include <QPainter>
#include <qpen.h>

using namespace pr;

#define RADIUS 5

PriMouseFollower::PriMouseFollower(QGraphicsView* view)
    : QGraphicsItem()
    , mp_view(view)
{
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsSelectable);

    setZValue(2.0);
    setVisible(true);
}

PriMouseFollower::~PriMouseFollower()
{

}

void PriMouseFollower::set_corrected_mouse_pos(const QPoint &mouse_pos)
{
    m_corrected_mouse_pos = mouse_pos;
}

QPoint pr::PriMouseFollower::global_mouse_pos() const
{
    return m_corrected_mouse_pos;
}

QPointF pr::PriMouseFollower::scene_pos() const
{
    auto local_pos = mp_view->mapFromGlobal(m_corrected_mouse_pos);
    QPointF scene_pos = mp_view->mapToScene(local_pos);
    return scene_pos;
}

void PriMouseFollower::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto draw_circle = [&](const QPointF& mid_point)
    {
        QColor color(0, 0, 0, 255);
        QPen pen(color, 1, Qt::DashLine);
        painter->setPen(pen);
        pen.setWidth(1);
        // 设置点的大小和位置（以视图坐标系为基准）
        int radius = RADIUS;
        int x = mapFromScene(mid_point).x();
        int y = mapFromScene(mid_point).y();

        // 保存当前的变换矩阵
        painter->save();

        // 重置变换矩阵，使点的大小不受视图变换的影响
        painter->resetTransform();

        painter->setPen(pen);

        // 绘制空心点
        QPointF center(x, y);
        painter->drawEllipse(center, radius, radius);
        painter->drawLine(center.x() - radius, center.y(), center.x() + radius, center.y());
        painter->drawLine(center.x(), center.y() - radius, center.x(), center.y() + radius);
        // 恢复变换矩阵
        painter->restore();
    };

    if (m_corrected_mouse_pos.x() >= 0 && m_corrected_mouse_pos.y() >= 0)
    {
        // 设置十字线的颜色和宽度
        draw_circle(scene_pos());
    }
}

QRectF pr::PriMouseFollower::boundingRect() const
{
    // 将视图坐标转换为场景坐标
    auto pos = scene_pos();
    QRectF rect(pos.x() - RADIUS, pos.y() - RADIUS, 2 * RADIUS, 2 * RADIUS);
    return rect;
}
