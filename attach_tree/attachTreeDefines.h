#ifndef ATTACHTREEDEFINES_H
#define ATTACHTREEDEFINES_H

// 定义 SAFE_DELETE 宏
#include "qglobal.h"

#define SAFE_DELETE(ptr) \
    if (ptr)            \
    {                    \
        delete ptr;      \
        ptr = nullptr;   \
    }

namespace at
{
// 定义AttachTreeNode的Id
typedef qint64 AttachTreeNodeId;
#define INVALID_ATTACH_TREE_NODE_ID   0
// 定义node type.
enum class NodeType : quint8
{
    LOCATION = 0,
    ADD,
    DELETE,
    NONE           // 使用此参数代表当前节点没有此无属性
};
// 定义node direction of one parent-attach-point
enum class NodeDirection : quint8
{
    TOP_LEFT = 0,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    TOP_RIGHT,
    NONE          // 使用此参数代表当前节点没有此无属性
};
}

#endif // ATTACHTREEDEFINES_H
