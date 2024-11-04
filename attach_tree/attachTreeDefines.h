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

// 定义node的delete行为
enum class NodeBooleanSubtractType : quint8
{
    NONE  = 0,
    ONLY_WITH_PARENT_NODE,              // 仅删除父节点： delete类型的节点只对自己的父节点产生作用。并且会忽略掉全部location类型的父节点，只对第一个不是location的父节点产生作用。
    WITH_PARENT_AND_BROTHER_NODE,       // 删除父节点+兄弟节点：
    WITH_PARENT_AND_CHILD_NODE,         // 删除父节点+子节点
};
}

#endif // ATTACHTREEDEFINES_H
