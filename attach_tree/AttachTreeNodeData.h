#ifndef ATTACHTREENODEDATA_H
#define ATTACHTREENODEDATA_H

#include "QMetaType"

namespace at
{
class AttachTreeBaseNode;
}

//定义结构体 TreeNode*和点的idx
struct TreeNodeData
{
    at::AttachTreeBaseNode* node_pointer;
    int point_idx_in_shape;
    int is_shape;
};
Q_DECLARE_METATYPE(TreeNodeData)

#endif // ATTACHTREENODEDATA_H
