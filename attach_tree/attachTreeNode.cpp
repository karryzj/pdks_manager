#include <AttachTreeRootNode.h>
#include <AttachTreeUtils.h>
#include <PointE.h>
#include <QGraphicsView>
#include <viewport.h>

#include "attachTreeNode.h"
#include "attachTreeDefines.h"

#include "shapeBase.h"
#include "shapeFactory.h"
#include "shapePointGraphicsItem.h"
#include "redoService.h"
#include "attachTreeNodeMgr.h"
#include "ParamDecl.h"

using namespace at;

AttachTreeNode::AttachTreeNode(AttachTreeBaseNode* parent,
                               int parent_attach_point_idx,               // 为-1的时候，此节点为根节点,
                               const QString& shape_name,                 // 图形类型
                               const QVector<pm::ParamDecl> & params,    // 所创建图形的参数
                               NodeType node_type,                        // 控制是辅助定位，还是增加、减去。
                               NodeDirection
                               node_dirction,                             // 放在局部坐标系的四个方位的哪一个
                               ly::LayerInfo* layer,                      // 图层信息，用于填充
                               bool connect_with_mw)                      // 用来绘制图形的GraphicsView
    : AttachTreeBaseNode()
    , mp_parent(parent)
    , m_parent_attach_point_idx(parent_attach_point_idx)
    , m_shape_name(shape_name)
    , m_node_type(node_type)
    , m_node_direction(node_dirction)
    , mp_layer_info(layer)
    , m_connect_with_mw(connect_with_mw)
{
    // 创建Shape
    sp::ShapePointGraphicsItem* parent_attach_point = nullptr;
    if(dynamic_cast<AttachTreeRootNode * >(mp_parent))
    {
        parent_attach_point = dynamic_cast<AttachTreeRootNode*>(mp_parent)->origin_point();
    }
    else
    {
        parent_attach_point = dynamic_cast<AttachTreeNode*>(mp_parent)->point_items()[parent_attach_point_idx];
    }

    mp_shape = sp::ShapeFactory::instance()->create_shape(shape_name, mp_parent->param_mgr(), params, parent_attach_point);
    mp_shape->shape_graphics_item()->set_layer_info(mp_layer_info);
    if(mp_parent->view())
    {
        mp_shape->update();
    }

    // 分配Id
    m_tree_node_id = parent->tree_node_mgr()->register_tree_node(this);

    connect(mp_parent->param_mgr(), &pm::ParamMgr::param_changed, this, &AttachTreeNode::on_param_changed);
}

AttachTreeNode::~AttachTreeNode()
{
    auto pm_mgr = param_mgr();

    // 把全部子对象都析构
    remove_all();

    // 从自己的父亲中移出自己
    mp_parent->remove_child(this);

    // 处理自身的对象
    auto* shape_item = mp_shape->shape_graphics_item();
    if (shape_item)
    {
        shape_item->setVisible(false);
        shape_item->setSelected(false);
        shape_item->update();
        if(view())
        {
            view()->scene()->removeItem(shape_item);
        }
        auto point_items = mp_shape->point_graphics_items();

        for (auto point_item : point_items)
        {
            point_item->setVisible(false);
            point_item->setSelected(false);
            point_item->update();
            if(view())
            {
                view()->scene()->removeItem(point_item);
            }
        }
    }
    SAFE_DELETE(mp_shape);

    tree_node_mgr()->unregister_tree_node(m_tree_node_id);

    disconnect(pm_mgr, &pm::ParamMgr::param_changed, this, &AttachTreeNode::on_param_changed);
}

sp::ShapeDrawGraphicsItem* AttachTreeNode::shape_item()
{
    return mp_shape->shape_graphics_item();
}

QVector<sp::ShapePointGraphicsItem*> at::AttachTreeNode::point_items() const
{
    return mp_shape->point_graphics_items();
}

void AttachTreeNode::set_node_type(NodeType node_type)
{
    auto old_node_type = m_node_type;

    if(m_node_type == node_type)
    {
        return;
    }

    m_node_type = node_type;
    tree_node_mgr()->on_set_node_type(m_tree_node_id);

    //for redo
    cm::RedoService::instance()->queue(this, new SetNodeTypeOp(old_node_type, m_node_type));
}

NodeType AttachTreeNode::node_type() const
{
    return m_node_type;
}

void AttachTreeNode::set_node_direction(NodeDirection node_direction)
{
    auto old_dire = m_node_direction;

    if(m_node_direction == node_direction)
    {
        return;
    }

    m_node_direction = node_direction;
    tree_node_mgr()->on_set_node_direction(m_tree_node_id);
    update();

    //for redo
    cm::RedoService::instance()->queue(this, new SetDirectionOp(old_dire, m_node_direction));
}

NodeDirection AttachTreeNode::node_direction() const
{
    return m_node_direction;
}

void AttachTreeNode::set_layer_info(ly::LayerInfo* layer_info)
{
    auto old_layer = mp_layer_info;

    if(mp_layer_info == layer_info)
    {
        return;
    }

    mp_layer_info = layer_info;
    shape_item()->set_layer_info(mp_layer_info);
    tree_node_mgr()->on_set_layer_info(m_tree_node_id);

    //for redo
    cm::RedoService::instance()->queue(this, new SetLayerOp(old_layer, mp_layer_info));
}

ly::LayerInfo* AttachTreeNode::layer_info() const
{
    return mp_layer_info;
}

const QVector<pm::ParamDecl> &AttachTreeNode::params() const
{
    return mp_shape->params();
}

AttachTreeNodeId AttachTreeNode::id() const
{
    return m_tree_node_id;
}

void AttachTreeNode::set_new_id()
{
    Q_ASSERT(m_tree_node_id != INVALID_ATTACH_TREE_NODE_ID);
    m_tree_node_id = tree_node_mgr()->allocate_tree_node_new_id(m_tree_node_id);
}

bool at::AttachTreeNode::connect_with_mw() const
{
    return m_connect_with_mw;
}

void AttachTreeNode::set_connect_with_mw(bool connect_with_mw)
{
    m_connect_with_mw = connect_with_mw;
}

void at::AttachTreeNode::on_param_changed(const QString &key)
{
    if(!mp_shape->param_used(key))return;

    update();
}

void AttachTreeNode::set_params(const QVector<pm::ParamDecl> & params)
{
    auto old_params = mp_shape->params();
    mp_shape->set_new_params(params);
    update();
    if(update_children())
    {
        tree_node_mgr()->on_set_params(m_tree_node_id);
    }
    //for redo
    cm::RedoService::instance()->queue(this, new SetParamsOp(old_params, params));
}

void AttachTreeNode::update()
{
    auto graphics_view = view();
    if(nullptr == graphics_view)
    {
        return;
    }

    // 将QGraphicsItem加入View中
    if(graphics_view->scene() != shape_item()->scene())  // 避免重复添加
    {
        graphics_view->scene()->addItem(shape_item());
        const auto& pt_items = point_items();
        for(auto pt_item : pt_items)
        {
            graphics_view->scene()->addItem(pt_item);
        }
    }

    /* 更新内容：
     * 1. 变换矩阵；
     * 2. 参数和附着点等的数据；
     * 3. 更新子节点;
     */
    update_trans();
    mp_shape->update();

    if(!update_children())
    {
        return;
    }

    const auto& child_items = children();
    for (auto itor = child_items.begin(); itor != child_items.end(); itor++)
    {
        for (auto & node : itor.value())
        {
            if (node)
            {
                node->update();
            }
        }
    }
}

void AttachTreeNode::update_trans()
{
    // 设置附着点相对局部坐标系的相对位置的偏移
    auto tree_node = dynamic_cast<AttachTreeNode*>(mp_parent);
    if(tree_node)
    {
        QPointF at_pt = AttachTreeUtils::attach_point(tree_node, get_parent_attach_point_idx(), false);
        QTransform trans = mp_parent->transform();
        trans.translate(at_pt.x(), at_pt.y());
        set_transform(trans);
    }
    else
    {
        QTransform trans;
        set_transform(trans);
    }

    // attch point exp
    if(tree_node)
    {
        pm::PointE point = AttachTreeUtils::attach_exp_point(tree_node, get_parent_attach_point_idx(), false);
        auto exp_trans = mp_parent->transorm_for_expression();
        exp_trans.translate(point.x(), point.y());
        set_transform_for_expression(exp_trans);
    }
    else
    {
        pm::PointE exp_p("0", "0");
        set_transform_for_expression(exp_p);
    }
}

void at::AttachTreeNode::undo(db::Op *op)
{
    SetParamsOp  *p_op = dynamic_cast<SetParamsOp*>(op);
    if(p_op)
    {
        auto params = p_op->old_params();
        mp_shape->set_new_params(params);
        tree_node_mgr()->on_set_params(m_tree_node_id);
    }

    SetLayerOp *l_op = dynamic_cast<SetLayerOp*>(op);
    if(l_op)
    {
        mp_layer_info = l_op->old_layer();
        shape_item()->set_layer_info(mp_layer_info);
        tree_node_mgr()->on_set_layer_info(m_tree_node_id);
    }
    SetNodeTypeOp *t_op = dynamic_cast<SetNodeTypeOp*>(op);
    if(t_op)
    {
        m_node_type = t_op->old_type();
        tree_node_mgr()->on_set_node_type(m_tree_node_id);
    }
    SetDirectionOp *d_op = dynamic_cast<SetDirectionOp*>(op);
    if(d_op)
    {
        m_node_direction = d_op->old_dire();
        tree_node_mgr()->on_set_node_direction(m_tree_node_id);
    }

    update();
    tree_node_mgr()->update();
}

void AttachTreeNode::redo(db::Op *op)
{
    SetParamsOp  *p_op = dynamic_cast<SetParamsOp*>(op);
    if(p_op)
    {
        auto params = p_op->new_params();
        mp_shape->set_new_params(params);
        tree_node_mgr()->on_set_params(m_tree_node_id);
    }

    SetLayerOp *l_op = dynamic_cast<SetLayerOp*>(op);
    if(l_op)
    {
        mp_layer_info = l_op->new_layer();
        shape_item()->set_layer_info(mp_layer_info);
        tree_node_mgr()->on_set_layer_info(m_tree_node_id);
    }
    SetNodeTypeOp *t_op = dynamic_cast<SetNodeTypeOp*>(op);
    if(t_op)
    {
        m_node_type = t_op->new_type();
        tree_node_mgr()->on_set_node_type(m_tree_node_id);
    }
    SetDirectionOp *d_op = dynamic_cast<SetDirectionOp*>(op);
    if(d_op)
    {
        m_node_direction = d_op->new_dire();
        tree_node_mgr()->on_set_node_direction(m_tree_node_id);
    }

    update();
    tree_node_mgr()->update();
}

pm::ParamMgr *at::AttachTreeNode::param_mgr() const
{
    return mp_parent->param_mgr();
}

QGraphicsView *at::AttachTreeNode::view() const
{
    return mp_parent->view();
}

AttachTreeNodeMgr *at::AttachTreeNode::tree_node_mgr() const
{
    return mp_parent->tree_node_mgr();
}

AttachTreeBaseNode *AttachTreeNode::parent_node() const
{
    return mp_parent;
}

void AttachTreeNode::set_new_parent_node(AttachTreeBaseNode *new_parent, int parent_index)
{
    mp_parent = new_parent;
    m_parent_attach_point_idx = parent_index;

    // 创建Shape
    sp::ShapePointGraphicsItem* parent_attach_point = nullptr;
    if(dynamic_cast<AttachTreeRootNode * >(mp_parent))
    {
        parent_attach_point = dynamic_cast<AttachTreeRootNode*>(mp_parent)->origin_point();
    }
    else
    {
        parent_attach_point = dynamic_cast<AttachTreeNode*>(mp_parent)->point_items()[m_parent_attach_point_idx];
    }
    mp_shape->set_parent_attach_point(parent_attach_point);

    // new_parent->add_child(this, parent_index); 这件事不在此处理
}

int AttachTreeNode::get_parent_attach_point_idx() const
{
    return m_parent_attach_point_idx;
}

sp::ShapeBase* AttachTreeNode::shape() const
{
    return mp_shape;
}

void AttachTreeNode::set_anchor_point(int attach_point_idx,
                                      bool is_anchor)
{
    QVector<sp::ShapePointGraphicsItem *>   point_items = mp_shape->point_graphics_items();
    Q_ASSERT( point_items.size() > attach_point_idx);
    point_items[attach_point_idx]->set_anchor_point(is_anchor);
}

bool AttachTreeNode::is_anchor_point(int attach_point_idx)
const
{
    QVector<sp::ShapePointGraphicsItem *> point_items = mp_shape->point_graphics_items();
    Q_ASSERT( point_items.size() > attach_point_idx);
    return point_items[attach_point_idx]->is_anchor_point();
}

void AttachTreeNode::set_coord_point(int attach_point_idx, bool is_coord)
{
    QVector<sp::ShapePointGraphicsItem *> point_items = mp_shape->point_graphics_items();
    Q_ASSERT( point_items.size() > attach_point_idx);
    point_items[attach_point_idx]->set_coor_anchor_point(is_coord);
}

bool AttachTreeNode::is_coord_point(int attach_point_idx) const
{
    QVector<sp::ShapePointGraphicsItem *> point_items = mp_shape->point_graphics_items();
    Q_ASSERT( point_items.size() > attach_point_idx);
    return point_items[attach_point_idx]->is_coor_anchor_point();
}

const QString &AttachTreeNode::shape_name() const
{
    return m_shape_name;
}
