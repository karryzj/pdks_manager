#include "attachTreePrivateUtils.h"

#include <AttachTreeNode.h>

using namespace at;

AttachTreeNode *at::AttachTreePrivateUtils::add_new_child(
    AttachTreeBaseNode *parent,
    int parent_attach_point_idx,
    const QString &shape_name,
    const QVector<pm::ParamDecl> &params,
    NodeType node_type, NodeDirection node_direction,  NodeBooleanSubtractType node_boolean_subtract_type,
    ly::LayerInfo *layer)
{
    auto child = new AttachTreeNode(parent, parent_attach_point_idx, shape_name, params, node_type, node_direction, node_boolean_subtract_type, layer);
    return child;
}
