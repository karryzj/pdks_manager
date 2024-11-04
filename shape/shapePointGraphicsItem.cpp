#include "shapePointGraphicsItem.h"
#include "shapeDefines.h"

#include <QGraphicsSceneMouseEvent>
#include <ShapeDrawGraphicsItem.h>
#include <qgraphicsview.h>
#include <qpainter.h>

using namespace sp;

ShapePointGraphicsItem*  ShapePointGraphicsItem::cachedItem = nullptr;

ShapePointGraphicsItem::ShapePointGraphicsItem(const QPointF& pos, ShapeDrawGraphicsItem* parent, int index, bool is_origin_point)
    : QGraphicsItem(nullptr) // 此处必须设置为没有父图形节点
    , m_pos(pos)
    , mp_shape_item(parent)
    , m_index(index)
    , m_is_origin_point(is_origin_point)
{
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsSelectable);
    // 初始化附着点的绘制对象
    m_shape_name = QString(SHAPE_POINT) + QString::number(m_index);
    setZValue(1.0);
    setVisible(true);
    //setPos(pos);
}

ShapePointGraphicsItem::~ShapePointGraphicsItem()
{
    if(cachedItem == this)
    {
        cachedItem = nullptr;
    }
}

QRectF ShapePointGraphicsItem::boundingRect() const
{
    // 定义项的边界
    return QRectF(m_pos.x() - SHAPE_POINT_BOUNDING_RECT_DEFAULT_VALUE / 2,
                  m_pos.y() - SHAPE_POINT_BOUNDING_RECT_DEFAULT_VALUE / 2,
                  SHAPE_POINT_BOUNDING_RECT_DEFAULT_VALUE,
                  SHAPE_POINT_BOUNDING_RECT_DEFAULT_VALUE);
}

bool sp::ShapePointGraphicsItem::is_anchor_point() const
{
    return m_is_anchor_point;
}

void ShapePointGraphicsItem::set_anchor_point(bool is_anchor_point)
{
    m_is_anchor_point = is_anchor_point;
}

const QString &ShapePointGraphicsItem::rotate_angle() const
{
    return m_rotate_angle;
}

void ShapePointGraphicsItem::set_rotate_angle(const QString &rotate_angle)
{
    m_rotate_angle = rotate_angle;
}

bool ShapePointGraphicsItem::is_coor_anchor_point() const
{
    return m_is_coor_anchor_point;
}

void ShapePointGraphicsItem::set_coor_anchor_point(bool is_coor_anchor_point)
{
    m_is_coor_anchor_point = is_coor_anchor_point;
}

void ShapePointGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);


    // 绘制点
    if (isSelected())
    {
        painter->setBrush(Qt::red);
        painter->setPen(QPen(Qt::yellow, SHAPE_POINT_HIGH_LIGHT_DEFAULT_VALUE)); // 高亮边框
        painter->drawEllipse(m_pos, SHAPE_POINT_BOUNDING_RECT_DEFAULT_VALUE / 2,
                             SHAPE_POINT_BOUNDING_RECT_DEFAULT_VALUE / 2);
    }
    else
    {
        if(m_is_origin_point)
        {
            painter->setBrush(Qt::white);
            painter->setPen(Qt::black);
            painter->drawEllipse(m_pos, SHAPE_POINT_BOUNDING_RECT_DEFAULT_VALUE / 3,
                                 SHAPE_POINT_BOUNDING_RECT_DEFAULT_VALUE / 3);
        }
        else
        {
            painter->setBrush(Qt::blue);
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(m_pos, SHAPE_POINT_BOUNDING_RECT_DEFAULT_VALUE / 2,
                                 SHAPE_POINT_BOUNDING_RECT_DEFAULT_VALUE / 2);
        }
    }
}

void ShapePointGraphicsItem::reset_mouse_move_info()
{
    if(m_mouse_move_info.rectangle_item.scene() != nullptr)
    {
        m_mouse_move_info.point_item = nullptr;
        m_mouse_move_info.has_been_moved = false;
        m_mouse_move_info.end_point = QPointF();
        m_mouse_move_info.rectangle_item.setRect(QRectF(pos(), pos()));
        scene()->removeItem(&m_mouse_move_info.rectangle_item);
    }
}

void ShapePointGraphicsItem::set_new_position(const QPointF &pos)
{
    m_pos = pos;
    reset_mouse_move_info();
    prepareGeometryChange();
    update();
}

const QString &ShapePointGraphicsItem::shape_name() const
{
    return m_shape_name;
}

ShapeDrawGraphicsItem *ShapePointGraphicsItem::shape_item() const
{
    return mp_shape_item;
}

const QPointF& ShapePointGraphicsItem::pos() const
{
    return m_pos;
}

void ShapePointGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // 调用基类实现以确保正常的选择行为
    QGraphicsItem::mousePressEvent(event);
    // 处理选中此点，鼠标移动时的绘制效果
    m_mouse_move_info.point_item = this;
    m_mouse_move_info.has_been_moved = false;
    scene()->addItem(&m_mouse_move_info.rectangle_item);

    // 处理高亮效果
    setVisible(true);
    setSelected(true);

    // 将当前项的指针添加到缓存中
    ShapePointGraphicsItem::cachedItem = this;
}

void ShapePointGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // 调用基类实现以确保正常的选择行为
    QGraphicsItem::mouseMoveEvent(event);

    // 处理选中此点，鼠标移动时的绘制效果
    if (m_mouse_move_info.point_item)
    {
        // 完成矩形的绘制
        m_mouse_move_info.end_point = event->scenePos();
        QRectF rect(m_mouse_move_info.point_item->pos(), m_mouse_move_info.end_point);
        m_mouse_move_info.rectangle_item.setRect(rect);
        scene()->update();
        m_mouse_move_info.has_been_moved = true;
    }
    return;
}

void ShapePointGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // 调用基类实现以确保正常的选择行为
    QGraphicsItem::mouseReleaseEvent(event);

    // 处理选中此点，鼠标移动时的绘制效果
    if(m_mouse_move_info.has_been_moved == true)
    {
        emit send_dragged_rectangle_info(this, m_mouse_move_info.end_point);
    }

    reset_mouse_move_info();

    // 将当前项的指针添加到缓存中
    ShapePointGraphicsItem::cachedItem = this;
}

void ShapePointGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QGraphicsItem::contextMenuEvent(event);
    setVisible(true);
    setSelected(true);
}
