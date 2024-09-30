#include <QWidget>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialog>
#include <QMenuBar>
#include <QAction>
#include <qcombobox.h>
#include <PriGraphicsScene.h>
#include <QGraphicsSceneContextMenuEvent>
#include <qapplication.h>
#include <QDebug>
#include <AttachTreeRootNode.h>

#include "priGraphicsView.h"
#include "AttachTreeNode.h"
#include "primitive.h"
#include "shapeBase.h"
#include "shapeDefines.h"
#include "viewport.h"
#include "attachTreeNodeMgr.h"
#include "primitiveDefines.h"
#include "attachTreeUtils.h"
#include "priUtils.h"

namespace pr
{
PriGraphicsView::PriGraphicsView(PriGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
    , m_scaleFactor(1.15)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);   // 视图变化时，让整个视图的内容都更新。
    setRenderHint(QPainter::Antialiasing);      // 启用抗锯齿
    setDragMode(QGraphicsView::RubberBandDrag);    // 使得能够进行拖拽
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);     // 仅重绘边界矩形

    m_click_timer.setSingleShot(true);
    bool succeed = connect(&m_click_timer, &QTimer::timeout, this, &PriGraphicsView::single_click_no_double_click);
    Q_ASSERT(succeed);
}

PriGraphicsView::~PriGraphicsView()
{
}

void PriGraphicsView::set_primitive(Primitive* pri)
{
    mp_pri = pri;
}

pr::Primitive *pr::PriGraphicsView::current_primitive() const
{
    return mp_pri;
}

void PriGraphicsView::align_point_items(const QString &align_type)
{
    // check input
    if(m_cache_point_items.empty())
    {
        return;
    }

    /* 这里假设了点都是同一个多边形的附着点或者父节点。
     * 如果不是，就不进行任何处理，直接返回，
     * 后续会考虑更多情况。
     * 此接口的性能可优化。
     */
    sp::ShapePointGraphicsItem* parent_attach_point = nullptr;
    QVector<sp::ShapePointGraphicsItem*> polygen_point_items;  // 至少有一个节点
    at::AttachTreeNode* polygen_tree_node = nullptr;

    // 先查找是不是多边形节点
    for(auto point_item : m_cache_point_items)
    {
        auto tree_node = at::AttachTreeUtils::attach_tree_node_point_item_in(point_item, mp_pri->at_root());
        if(nullptr == tree_node)  // 异常情况
        {
            return;
        }

        at::AttachTreeNode* shape_node = dynamic_cast<at::AttachTreeNode*>(tree_node);
        if(!shape_node)
        {
            continue;
        }

        if(shape_node->shape_name() != SHAPE_POLYGEN)
        {
            continue;
        }

        polygen_tree_node = shape_node;
    }

    if(nullptr == polygen_tree_node)
    {
        return;
    }

    for(auto point_item : m_cache_point_items)
    {
        // 先检查是不是父节点
        if(polygen_tree_node->shape()->parent_attach_point() == point_item)
        {
            parent_attach_point = point_item;
            continue;
        }

        auto tree_node = at::AttachTreeUtils::attach_tree_node_point_item_in(point_item, mp_pri->at_root());
        if(tree_node != polygen_tree_node)
        {
            return;
        }
        polygen_point_items.push_back(point_item);
    }

    // 参数化的情况也处理不了
    const auto& params = polygen_tree_node->params();
    for(const auto& param : params)
    {
        if(param.key() != SHAPE_ROTATE && param.is_expression(polygen_tree_node->param_mgr()))
        {
            return;
        }
    }

    auto point_item_logic_pos = [&](sp::ShapePointGraphicsItem* point_item)
    {
        QPointF logic_pos =  polygen_tree_node->tree_node_mgr()->viewport()->map_from_scene(point_item->pos());
        return logic_pos;
    };

    auto local_logic_origin_point = point_item_logic_pos(polygen_tree_node->shape()->parent_attach_point());

    // HINT@leixunyong。这里有代码复用的问题。
    if(align_type == PRI_LEFT_ALIGN_ACTION_NAME)
    {
        if(parent_attach_point)
        {
            auto standard_pos = point_item_logic_pos(parent_attach_point);
            for(auto point_item : polygen_point_items)
            {
                auto item_logic_pos =  point_item_logic_pos(point_item);
                if(item_logic_pos.x() < standard_pos.x())
                {
                    continue;
                }
                item_logic_pos.setX(standard_pos.x());
                item_logic_pos = item_logic_pos - local_logic_origin_point;
                setup_point_item_param_by_pos(item_logic_pos, point_item);
            }
        }
        else
        {
            if(polygen_point_items.size() < 2)
            {
                return;
            }
            auto standard_pos = point_item_logic_pos(polygen_point_items[0]);
            for(auto point_item : polygen_point_items)
            {
                auto item_logic_pos =  point_item_logic_pos(point_item);
                if(item_logic_pos.x() < standard_pos.x())
                {
                    standard_pos = item_logic_pos;
                }
            }

            for(auto point_item : polygen_point_items)
            {
                auto item_logic_pos =  point_item_logic_pos(point_item);
                item_logic_pos.setX(standard_pos.x());
                item_logic_pos = item_logic_pos - local_logic_origin_point;
                setup_point_item_param_by_pos(item_logic_pos, point_item);
            }
        }
    }

    if(align_type == PRI_RIGHT_ALIGN_ACTION_NAME)
    {
        if(parent_attach_point)
        {
            auto standard_pos = point_item_logic_pos(parent_attach_point);
            for(auto point_item : polygen_point_items)
            {
                auto item_logic_pos =  point_item_logic_pos(point_item);
                if(item_logic_pos.x() > standard_pos.x())
                {
                    continue;
                }
                item_logic_pos.setX(standard_pos.x());
                item_logic_pos = item_logic_pos - local_logic_origin_point;
                setup_point_item_param_by_pos(item_logic_pos, point_item);
            }
        }
        else
        {
            if(polygen_point_items.size() < 2)
            {
                return;
            }
            auto standard_pos = point_item_logic_pos(polygen_point_items[0]);
            for(auto point_item : polygen_point_items)
            {
                auto item_logic_pos =  point_item_logic_pos(point_item);
                if(item_logic_pos.x() > standard_pos.x())
                {
                    standard_pos = item_logic_pos;
                }
            }

            for(auto point_item : polygen_point_items)
            {
                auto item_logic_pos =  point_item_logic_pos(point_item);
                item_logic_pos.setX(standard_pos.x());
                item_logic_pos = item_logic_pos - local_logic_origin_point;
                setup_point_item_param_by_pos(item_logic_pos, point_item);
            }
        }
    }

    if(align_type == PRI_TOP_ALIGN_ACTION_NAME)
    {
        if(parent_attach_point)
        {
            auto standard_pos = point_item_logic_pos(parent_attach_point);
            for(auto point_item : polygen_point_items)
            {
                auto item_logic_pos =  point_item_logic_pos(point_item);
                if(item_logic_pos.y() > standard_pos.y())
                {
                    continue;
                }
                item_logic_pos.setY(standard_pos.y());
                item_logic_pos = item_logic_pos - local_logic_origin_point;
                setup_point_item_param_by_pos(item_logic_pos, point_item);
            }
        }
        else
        {
            if(polygen_point_items.size() < 2)
            {
                return;
            }
            auto standard_pos = point_item_logic_pos(polygen_point_items[0]);
            for(auto point_item : polygen_point_items)
            {
                auto item_logic_pos =  point_item_logic_pos(point_item);
                if(item_logic_pos.y() > standard_pos.y())
                {
                    standard_pos = item_logic_pos;
                }
            }

            for(auto point_item : polygen_point_items)
            {
                auto item_logic_pos =  point_item_logic_pos(point_item);
                item_logic_pos.setY(standard_pos.y());
                item_logic_pos = item_logic_pos - local_logic_origin_point;
                setup_point_item_param_by_pos(item_logic_pos, point_item);
            }
        }
    }

    if(align_type == PRI_BOTTOM_ALIGN_ACTION_NAME)
    {
        if(parent_attach_point)
        {
            auto standard_pos = point_item_logic_pos(parent_attach_point);
            for(auto point_item : polygen_point_items)
            {
                auto item_logic_pos =  point_item_logic_pos(point_item);
                if(item_logic_pos.y() < standard_pos.y())
                {
                    continue;
                }
                item_logic_pos.setY(standard_pos.y());
                item_logic_pos = item_logic_pos - local_logic_origin_point;
                setup_point_item_param_by_pos(item_logic_pos, point_item);
            }
        }
        else
        {
            if(polygen_point_items.size() < 2)
            {
                return;
            }
            auto standard_pos = point_item_logic_pos(polygen_point_items[0]);
            for(auto point_item : polygen_point_items)
            {
                auto item_logic_pos =  point_item_logic_pos(point_item);
                if(item_logic_pos.y() < standard_pos.y())
                {
                    standard_pos = item_logic_pos;
                }
            }

            for(auto point_item : polygen_point_items)
            {
                auto item_logic_pos =  point_item_logic_pos(point_item);
                item_logic_pos.setY(standard_pos.y());
                item_logic_pos = item_logic_pos - local_logic_origin_point;
                setup_point_item_param_by_pos(item_logic_pos, point_item);
            }
        }
    }
}


void PriGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);

    QPoint pos = event->pos();
    // 将视图坐标转换为场景坐标
    QPointF scenePos = mapToScene(pos);
    emit mouseMoved(scenePos);

    // 右侧区域的阈值，比如右侧10%区域
    int rightAreaThreshold = width() * 0.1;
    if (event->pos().x() < width() - rightAreaThreshold)
    {
        emit mouseLeaveRightArea();
    }
}

void PriGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    QGraphicsView::contextMenuEvent(event);
    // 将事件位置转换为场景坐标
    QPointF scene_pos = mapToScene(event->pos());
    QPoint global_pos = event->globalPos();
    // 获取该位置的 QGraphicsItem
    QVector<QGraphicsItem*> clicked_items;
    {
        const auto& items = scene()->items();
        for(const auto& item : items)
        {
            if(dynamic_cast<sp::ShapeDrawGraphicsItem * >(item) == nullptr && dynamic_cast<sp::ShapePointGraphicsItem * >(item) == nullptr)
            {
                continue;
            }

            QRectF range = item->boundingRect();
            if(range.contains(scene_pos))
            {
                clicked_items.push_back(item);
            }
        }
    }

    sp::ShapeDrawGraphicsItem * shape_item = nullptr;
    sp::ShapePointGraphicsItem *  point_item = nullptr;
    for(auto p : clicked_items)
    {
        if(nullptr == shape_item)
        {
            shape_item = dynamic_cast<sp::ShapeDrawGraphicsItem*>(p);
        }

        if(nullptr == point_item)
        {
            point_item = dynamic_cast<sp::ShapePointGraphicsItem*>(p);
        }
    }

    if (point_item)
    {
        emit mouse_right_button_press(global_pos, point_item);
    }
    else if(shape_item)
    {
        emit mouse_right_button_press(global_pos, shape_item);
    }
}

void PriGraphicsView::wheelEvent(QWheelEvent *event)
{
    QGraphicsView::wheelEvent(event);
    int delta = event->delta();
    QPointF point = mp_pri->tree_node_mgr()->viewport()->map_from_scene(event->position());

    if (delta > 0)
    {
        mp_pri->tree_node_mgr()->viewport()->zoom_in(point, delta);
    }
    else
    {
        mp_pri->tree_node_mgr()->viewport()->zoom_out(point, -delta);

    }

    update();
}

void PriGraphicsView::keyPressEvent(QKeyEvent *event)
{
    QGraphicsView::keyPressEvent(event);
    if(event->key() == Qt::Key_Left)
    {
        mp_pri->tree_node_mgr()->viewport()->pan_left();
    }
    else if(event->key() == Qt::Key_Right)
    {
        mp_pri->tree_node_mgr()->viewport()->pan_right();
    }
    else if(event->key() == Qt::Key_Up)
    {
        mp_pri->tree_node_mgr()->viewport()->pan_up();
    }
    else if(event->key() == Qt::Key_Down)
    {
        mp_pri->tree_node_mgr()->viewport()->pan_down();
    }


    if(event->key() == Qt::Key_Escape)
    {
        emit key_escape_press();
    }
}

void PriGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);

    if(event->button() == Qt::LeftButton)
    {
        // 将事件位置转换为场景坐标
        QPointF scene_pos = mapToScene(event->pos());
        // 获取该位置的 QGraphicsItem
        QList<QGraphicsItem *> items_at_pos = scene()->items();
        sp::ShapeDrawGraphicsItem * shape_item = nullptr;
        sp::ShapePointGraphicsItem *  point_item = nullptr;

        pr::PriUtils::classify_graphics_items(mp_pri, scene_pos, items_at_pos, shape_item, point_item);

        if (point_item)
        {
            point_item->setVisible(true);
            point_item->setSelected(true);
            emit mouse_left_button_press_item(point_item);
        }
        else if(shape_item)
        {
            shape_item->setVisible(true);
            shape_item->set_point_items_visible(true);
            emit mouse_left_button_press_item(shape_item);
        }
        else
        {
            m_cache_point_items.clear();
            for(const auto& item : items_at_pos)
            {
                if(auto point_item = dynamic_cast<sp::ShapePointGraphicsItem * >(item)  ) // 点击的时候摸到了图形
                {
                    if(point_item->shape_item() != nullptr)
                    {
                        item->setVisible(false);
                    }
                    else
                    {
                        item->setVisible(true);  // HINT@leixunyong。这里这么干是因为不进行额外设置的话，会导致不可见。
                    }
                }
            }

            // HINT@leixunyong。在什么图形都没有选中的时候，检查是否按下了ctrl键，并根据ctrl键来增加一个矩形
            // 检查是否按住了 Ctrl 键
            if (event->modifiers() & Qt::ControlModifier)
            {
                // 在场景中添加一个圆形
                auto cache = sp::ShapePointGraphicsItem::cachedItem;
                sp::ShapePointGraphicsItem::cachedItem = mp_pri->at_root()->origin_point();
                emit mp_pri->at_root()->origin_point()->send_dragged_rectangle_info(mp_pri->at_root()->origin_point(), scene_pos);
                sp::ShapePointGraphicsItem::cachedItem = cache;
            }

        }
        mp_pri->at_root()->origin_point()->setVisible(true); // HINT@leixunyong。这里这么干是因为不进行额外设置的话，会导致不可见。
        // 设置单击事件的延迟。这里的逻辑是，在200ms内，如果没有触发双击事件，那么就会执行超时事件。否则就什么都不做。
        m_is_double_click = false;
        m_cache_single_click_pos = event->pos();
        m_click_timer.start(200);
    }
}

void PriGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{

    if (dragMode() == QGraphicsView::RubberBandDrag)
    {
        m_cache_point_items.clear();
        // 获取框选区域
        QRect rubberBandRect = this->rubberBandRect();
        if (!rubberBandRect.isEmpty())
        {
            // 获取框选区域内的所有项
            QList<QGraphicsItem *> selectedItems = scene()->items(rubberBandRect);

            // 打印选中的项
            for (QGraphicsItem *item : selectedItems)
            {
                sp::ShapePointGraphicsItem* point_item = dynamic_cast<sp::ShapePointGraphicsItem*>(item);
                if (point_item)
                {
                    point_item->setVisible(true);
                    point_item->setSelected(true);
                    m_cache_point_items.push_back(point_item);
                }
            }
        }
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void PriGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsView::mouseDoubleClickEvent(event);

    if(event->button() == Qt::LeftButton)
    {
        m_is_double_click = true;
        m_click_timer.stop();  // 停止单击事件的计时器。
        emit double_click_mouse_left_button();

        // 检查是否是双击了一个图形
        // 获取相对于视图的坐标
        QPoint view_pos = event->pos();
        QPointF scene_pos = mapToScene(view_pos);

        const auto& items = scene()->items();
        sp::ShapeDrawGraphicsItem* shape_item = nullptr;
        sp::ShapePointGraphicsItem* point_item = nullptr;
        PriUtils::classify_graphics_items(mp_pri, scene_pos, items, shape_item, point_item);
        if(shape_item)
        {
            emit shape_item->item_double_clicked(shape_item);
        }
    }
}

void PriGraphicsView::single_click_no_double_click()
{
    if(!m_is_double_click)
    {
        emit mouse_left_button_press(m_cache_single_click_pos);
    }
}

void PriGraphicsView::setup_point_item_param_by_pos(const QPointF &pos, sp::ShapePointGraphicsItem *point_item)
{
    auto tree_node = dynamic_cast<at::AttachTreeNode*>(at::AttachTreeUtils::attach_tree_node_point_item_in(point_item, mp_pri->at_root()) );
    if(nullptr == tree_node)
    {
        return;
    }

    if(tree_node ->shape_name() != SHAPE_POLYGEN )
    {
        return;
    }

    int point_idx_pos = at::AttachTreeUtils::point_item_index_in_tree_node(tree_node, point_item);
    int param_x_idx = 1 + point_idx_pos * 2;
    int param_y_idx = param_x_idx + 1;

    auto params = tree_node->params();
    if(params.size() <= param_x_idx || params.size() <= param_y_idx)
    {
        return;
    }

    params[param_x_idx].set_expression(QString::number(pos.x()));
    params[param_x_idx].set_value(pos.x());

    params[param_y_idx].set_expression(QString::number(pos.y()));
    params[param_y_idx].set_value(pos.y());

    tree_node->set_params(params);
}

}
