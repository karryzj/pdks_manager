#include "attachTreeNodeMgr.h"

using namespace at;

AttachTreeNodeMgr::AttachTreeNodeMgr()
    : QObject()
{

}

AttachTreeNodeMgr::~AttachTreeNodeMgr()
{

}

at::AttachTreeNodeId at::AttachTreeNodeMgr::allocate_new_id()
{
    static AttachTreeNodeId tree_node_id = INVALID_ATTACH_TREE_NODE_ID;
    return ++tree_node_id;
}
