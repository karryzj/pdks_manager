#ifndef ATTACHTREENODEMGR_H
#define ATTACHTREENODEMGR_H

#include "attachTreeDefines.h"
#include "atCommon.h"

#include <QObject.h>

namespace cm
{

class Viewport;

}

namespace at
{
/* HINT: 这个类的作用是把AttachTreeNode的set接口包了一层，并且加上了一定管理。
 * 目的是为了在TreeNode进行增、删、查、改的时候，能够迅速把相对应的信息整理出来。
 * 在对应业务需要使用的时候，直接从当前类的实例中取出对应的信息即可，而不用去遍历
 * 根节点取出各种信息。这样做的原因是，用空间换取时间。
 * 另外因为模块依赖的关系，不得不用多态去实现这个管理类。因此此类在定义的时候是个
 * 纯虚类，实际上的代码逻辑都在primitive模块中进行处理。
 */
class AttachTreeNode;
class AttachTreeRootNode;
class AT_PUBLIC AttachTreeNodeMgr : public QObject
{
public:
    AttachTreeNodeMgr();
    virtual ~AttachTreeNodeMgr();
    virtual void update() = 0;

    virtual bool is_disable() const = 0;
    virtual void set_disable(bool is_disable) = 0;

public:
    // 注册&反注册
    virtual int id_number() = 0;
    virtual AttachTreeNodeId register_tree_node(AttachTreeNode* new_tree_node) = 0;
    virtual void unregister_tree_node(const AttachTreeNodeId& tree_node_id) = 0;

    virtual AttachTreeNode* query(const AttachTreeNodeId& tree_node_id) = 0;

    virtual void set_tree_root_node(AttachTreeRootNode* root_node) = 0;

    virtual void init_viewport(double dbu) = 0;
    virtual cm::Viewport *viewport() = 0;

    virtual AttachTreeNodeId allocate_tree_node_new_id(const AttachTreeNodeId& tree_node_id) = 0;
protected:
    // HINT:这个接口放在这里是为了后续可能AttachTreeNodeId会转为一个类，并附带其他功能提供扩充，但是不影响接口的使用。
    at::AttachTreeNodeId allocate_new_id();
};
}

#endif // ATTACHTREENODEMGR_H
