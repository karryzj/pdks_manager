#include "attachTreeRootNode.h"
#include "attachTreeNode.h"
#include "attachTreeNodeMgr.h"
#include "qdebug.h"
#include "shapedrawgraphicsitem.h"
#include "viewport.h"
#include <ShapePointGraphicsItem.h>
#include <QGraphicsView>
#include <AttachTreePrivateUtils.h>

using namespace at;

AttachTreeRootNode::AttachTreeRootNode(pm::ParamMgr *param_mgr, QGraphicsView *view, AttachTreeNodeMgr* tree_node_mgr)
    : AttachTreeBaseNode()
    , mp_param_mgr(param_mgr)
    , mp_view(view)
    , mp_tree_node_mgr(tree_node_mgr)
    , m_coordinate_pos(std::make_pair(this, 0)) // 初始化时将 this 作为第一个参数
    , m_is_coord(true)
{
    // HINT: AttachTreeRootNode视为根节点，没有对应的Shape.
    mp_origin_point = new sp::ShapePointGraphicsItem(transform().map(QPointF(0, 0)), nullptr, 0, true);
    if(mp_tree_node_mgr)
    {
        mp_tree_node_mgr->set_tree_root_node(this);
    }

    if(mp_view)
    {
        mp_view->scene()->addItem(mp_origin_point);
    }
}

AttachTreeRootNode::~AttachTreeRootNode()
{
    if(mp_view)
    {
        mp_view->scene()->removeItem(mp_origin_point);
    }
    delete(mp_origin_point);
}

pm::ParamMgr *AttachTreeRootNode::param_mgr() const
{
    return mp_param_mgr;
}

QGraphicsView *AttachTreeRootNode::view() const
{
    return mp_view;
}

AttachTreeNodeMgr *AttachTreeRootNode::tree_node_mgr() const
{
    return mp_tree_node_mgr;
}


void AttachTreeRootNode::set_coordinate_pos(at::AttachTreeBaseNode* node, int point_idx)
{
    if(nullptr == node)
    {
        return;
    }

    m_coordinate_pos.first = node;
    m_coordinate_pos.second = point_idx;
}

std::pair<at::AttachTreeBaseNode*, int> AttachTreeRootNode::get_coordinate_pos() const
{
    return m_coordinate_pos;
}

sp::ShapePointGraphicsItem *AttachTreeRootNode::origin_point() const
{
    return mp_origin_point;
}

void AttachTreeRootNode::set_new_graphics_view(QGraphicsView* view)
{
    if(view == mp_view)
    {
        return;
    }

    if(mp_view)
    {
        qDebug() << "Warning: add root tree node to different graphics view";
    }

    mp_view = view;
    if(mp_view)
    {
        auto scene =  view->scene();
        scene->addItem(mp_origin_point);
        if(children().empty())
        {
            return;
        }

        QVector<AttachTreeNode*> ready_to_update_nodes = {children().begin().value()};
        while(!ready_to_update_nodes.empty())
        {
            QVector<AttachTreeNode*> next_nodes;
            for(auto node : ready_to_update_nodes)
            {
                auto shape_item = node->shape_item();
                scene->addItem(shape_item);
                auto point_items = node->point_items();
                for(auto point_item : point_items)
                {
                    scene->addItem(point_item);
                }

                const QMap<int, QVector<at::AttachTreeNode *> > & mp = node->children();
                for(auto itor = mp.begin(); itor != mp.end(); itor++)
                {
                    const auto& children = itor.value();
                    next_nodes.append(children);
                }
            }
            qSwap(next_nodes, ready_to_update_nodes);
        }
    }
}

void AttachTreeRootNode::update()
{
    auto graphics_view = view();
    if(graphics_view && graphics_view->scene() != mp_origin_point->scene())  // 避免重复添加
    {
        graphics_view->scene()->addItem(mp_origin_point);
    }

    const QMap<int, QVector<AttachTreeNode*>>& child_items = children();
    if(child_items.count() != 1)
    {
        return;
    }

    if(!update_children())
    {
        return;
    }

    const QVector<AttachTreeNode*>& children = child_items.begin().value();
    for(auto tree_node : children)
    {
        tree_node->update();
    }
}

void AttachTreeRootNode::update_trans()
{
    // HINT@leixunyong。现在还没发现要做什么事情。
}

void AttachTreeRootNode::set_coord_point(int attach_point_idx, bool is_coord)
{
    m_is_coord = is_coord;
}
bool AttachTreeRootNode::is_coord_point(int attach_point_idx) const
{
    return m_is_coord;
}

