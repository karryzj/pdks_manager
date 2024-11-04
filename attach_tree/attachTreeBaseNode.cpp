#include <QStack>
#include "attachTreeBaseNode.h"

#include <AttachTreeNode.h>
#include <AttachTreePrivateUtils.h>

#include "attachTreeNodeMgr.h"
#include "dbObject.h"

#include "shapeBase.h"
#include "pointE.h"



using namespace at;

AttachTreeBaseNode::AttachTreeBaseNode(QObject* parent)
    : QObject{parent}
      // , db::Object()
{

}

AttachTreeBaseNode::~AttachTreeBaseNode()
{

}

const QTransform& at::AttachTreeBaseNode::transform() const
{
    return m_trans;
}

const pm::PointE &at::AttachTreeBaseNode::transorm_for_expression() const
{
    return m_exp_trans;
}

const QMap<int, QVector<at::AttachTreeNode *> > &at::AttachTreeBaseNode::children() const
{
    return m_children;
}

at::AttachTreeNode *at::AttachTreeBaseNode::add_child(
    int parent_attach_point_idx,
    const QString &shape_name,
    const QVector<pm::ParamDecl> &params,
    NodeType node_type,
    NodeDirection node_direction,
    NodeBooleanSubtractType node_boolean_subtract_type,
    ly::LayerInfo *layer)
{
    // 初始化新节点
    AttachTreeNode* child = AttachTreePrivateUtils::add_new_child(
                                this,
                                parent_attach_point_idx,
                                shape_name,
                                params,
                                node_type,
                                node_direction,
                                node_boolean_subtract_type,
                                layer);

    add_child(child, parent_attach_point_idx);
    return child;
}

void at::AttachTreeBaseNode::add_child(
    at::AttachTreeNode* child,
    int parent_attach_point_idx)
{
    // 将新增节点放入m_children中进行更新
    auto itor = m_children.find(parent_attach_point_idx);
    if (itor == m_children.end())   // 新增
    {
        QVector<AttachTreeNode*> new_tree_nodes {child};
        m_children.insert(parent_attach_point_idx, new_tree_nodes);
    }
    else
    {
        itor.value().append(child);
    }

    update();
    return;
}


void at::AttachTreeBaseNode::set_transform(const QTransform& transfrom)
{
    m_trans = transfrom;
}

void at::AttachTreeBaseNode::set_transform_for_expression(const pm::PointE &transfrom)
{
    m_exp_trans = transfrom;
}

void at::AttachTreeBaseNode::remove_all()
{
    // 将自己的全部子节点都移除
    auto origin_status =  tree_node_mgr()->is_disable();
    tree_node_mgr()->set_disable(true);
    for (auto itor = m_children.begin(); itor != m_children.end(); itor++)
    {
        while(!itor.value().isEmpty())
        {
            auto node = itor.value()[0];
            itor.value().remove(0);
            SAFE_DELETE(node);
        }
    }
    m_children.clear();
    tree_node_mgr()->set_disable(origin_status);
    tree_node_mgr()->update();
}

void at::AttachTreeBaseNode::remove_child(AttachTreeNode* node)
{
    if(!node)
    {
        return;
    }

    bool ret = m_children[node->get_parent_attach_point_idx()].removeOne(node);
    if (m_children[node->get_parent_attach_point_idx()].size() == 0)
    {
        m_children.remove(node->get_parent_attach_point_idx());
    }
}

QRectF at::AttachTreeBaseNode::bounding_rect()
{
    QRectF rect = mp_shape ? mp_shape->shape_graphics_item()->boundingRect() : QRectF();

    for (auto itor = children().begin(); itor != children().end(); itor++)
    {
        for (auto & node : itor.value())
        {
            rect = rect.united(node->bounding_rect());
        }
    }
    return rect;
}

void at::AttachTreeBaseNode::traversal_subtree(const std::function<void(AttachTreeBaseNode*)>& proc)
{
    QStack<AttachTreeBaseNode*> node_stack;
    node_stack.push(this);  // 初始时将当前节点推入栈

    while (!node_stack.isEmpty())
    {
        // 取出栈顶的节点
        AttachTreeBaseNode* current_node = node_stack.pop();

        // 对当前节点执行 proc 操作
        proc(current_node);

        // 遍历当前节点的子节点并将它们推入栈中
        for(auto itor = current_node->m_children.begin(); itor != current_node->m_children.end(); ++itor)
        {
            const QVector<at::AttachTreeNode*>& child_tree_nodes = itor.value();
            // 将所有子节点按顺序压入栈
            for (AttachTreeBaseNode * child : child_tree_nodes)
            {
                node_stack.push(child);
            }
        }
    }
}

bool at::AttachTreeBaseNode::update_children() const
{
    return m_update_children;
}

void at::AttachTreeBaseNode::set_update_children(bool update_children)
{
    m_update_children = update_children;
}

