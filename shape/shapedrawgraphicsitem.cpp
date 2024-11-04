#include "shapeDrawGraphicsItem.h"

#include <QGraphicsSceneContextMenuEvent>
#include <layerUtils.h>
#include <qpainter.h>
#include "layInfo.h"
#include "shapeBase.h""

#include <qdebug.h>

using namespace sp;

sp::ShapeDrawGraphicsItem::ShapeDrawGraphicsItem(sp::ShapeBase* shape)
    : QObject()
    , QGraphicsItem(nullptr)  // 此处必须设置为没有父图形节点
    , mp_shape(shape)
{
    // 设置项的可选中性和可聚焦性
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setVisible(true);
    setZValue(0.0);
}

sp::ShapeDrawGraphicsItem::~ShapeDrawGraphicsItem()
{
}

void ShapeDrawGraphicsItem::set_painter_path(const QPainterPath &path)
{
    m_actual_draw_path = path;
    prepareGeometryChange();
    update();
}

void sp::ShapeDrawGraphicsItem::set_paint_type(PaintType paint_type)
{
    m_paint_type = paint_type;
    update();
}

void sp::ShapeDrawGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->save();
    // 启用抗锯齿
    if(m_paint_type == PaintType::DASH)
    {
        // 设置画笔为虚线
        QPen pen(Qt::darkGray);
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->drawPath(m_actual_draw_path);
    }
    else if(m_paint_type == PaintType::DRAW)
    {
        if(!mp_layer_info->visible())
        {
            return;
        }
        ly::layerUtils::setup_painter_draw_attribute_layer_info(mp_layer_info, * painter);
        //painter->setRenderHint(QPainter::Antialiasing, false);
        painter->drawPath(m_actual_draw_path);
        painter->setRenderHint(QPainter::Antialiasing, false);
        ly::layerUtils::setup_painter_fill_attribute_layer_info(mp_layer_info, * painter);
        painter->drawPath(m_actual_draw_path);
    }

    painter->restore();
}

QRectF sp::ShapeDrawGraphicsItem::boundingRect() const
{
    QRectF rect = m_actual_draw_path.boundingRect();
    return rect;
}

const QString& sp::ShapeDrawGraphicsItem::shape_name() const
{
    return mp_shape->shape_type();
}

void ShapeDrawGraphicsItem::set_layer_info(const ly::LayerInfo* lay_info)
{
    mp_layer_info = lay_info;
}

QVector<ShapePointGraphicsItem*> sp::ShapeDrawGraphicsItem::get_point_items() const
{
    return mp_shape->point_graphics_items();
}

ShapeBase *ShapeDrawGraphicsItem::shape_info() const
{
    return mp_shape;
}

void sp::ShapeDrawGraphicsItem::set_shape_info(sp::ShapeBase* shape)
{
    mp_shape = shape;
}

void ShapeDrawGraphicsItem::set_point_items_visible(bool visible) const
{
    // 显示全部的点
    QVector<ShapePointGraphicsItem*> point_items = get_point_items();
    for (auto * pt_item : point_items)
    {
        pt_item->setVisible(visible);
    }

    // 显示父节点
    auto parent_attach_point = mp_shape->parent_attach_point();
    if(parent_attach_point)
    {
        parent_attach_point->setVisible(visible);
    }
}

void sp::ShapeDrawGraphicsItem::set_shape_name(const QString& shape_name)
{
    m_shape_item_name = shape_name;
}

void sp::ShapeDrawGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mousePressEvent(event);
    if (event->button() == Qt::LeftButton)
    {
        set_point_items_visible(true);
    }
}

QVariant sp::ShapeDrawGraphicsItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == QGraphicsItem::ItemSelectedChange)
    {
        bool selected = value.toBool();
        if (!selected)
        {
            set_point_items_visible(false);
        }
        else
        {
            set_point_items_visible(true);
            // 发送信号给treeWidget，以便更新树形控件
            //emit pointItemChanged(this);
        }
    }
    return QGraphicsItem::itemChange(change, value);
}
