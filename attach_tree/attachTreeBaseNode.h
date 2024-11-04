#ifndef ATTACHTREEBASENODE_H
#define ATTACHTREEBASENODE_H

#include "QObject.h"
#include "atCommon.h"
#include "attachTreeDefines.h"

#include <pointE.h>
#include <qmap.h>
#include <qtransform.h>

#include "dbObject.h"
#include "ParamDecl.h"

namespace sp
{
class ShapeBase;
}

namespace pm
{
class ParamMgr;
class PointE;
}

namespace ly
{
class LayerInfo;
}

class QGraphicsView;

namespace at
{
class AttachTreeNodeMgr;
class AttachTreeNode;  // HINT@leixunyong。这么写不太符合设计规则，不够美观，但是从功能上看是难以避免的。
class AT_PUBLIC AttachTreeBaseNode : public QObject
{
    Q_OBJECT
public:
    explicit AttachTreeBaseNode(QObject* parent = nullptr);
    ~AttachTreeBaseNode() override;

public:
    virtual void update_trans() = 0;
    virtual pm::ParamMgr* param_mgr() const = 0;
    virtual QGraphicsView* view() const = 0;
    virtual AttachTreeNodeMgr* tree_node_mgr() const = 0;
    virtual void update() = 0;


    // virtual void undo (db::Op * /*op*/) {}
    // virtual void redo (db::Op * /*op*/) {}

public:
    const QTransform& transform() const;
    const pm::PointE& transorm_for_expression() const;
    // get tree node children
    const QMap<int, QVector<AttachTreeNode*>> & children() const;

    // 新增节点。在索引为parent_attach_point_idx的附着点所在位置，增加一个shape_name类型的Shape，这个Shape会放置在附着点的node_direction方位，以node_type形式进行增加，所在图层为layer
    AttachTreeNode* add_child(int parent_attach_point_idx,
                              const QString& shape_name,
                              const QVector<pm::ParamDecl> & params,
                              NodeType node_type,
                              NodeDirection node_direction,
                              NodeBooleanSubtractType node_boolean_subtract_type,
                              ly::LayerInfo* layer);

    void add_child(AttachTreeNode* new_child_tree_node, int point_index);

    // 移除全部节点并进行更新
    void remove_all();
    // 移除单个TreeNode
    void remove_child(AttachTreeNode* node);

    // me and children
    QRectF bounding_rect();

    // 递归访问所有当前节点下的子节点，模块自己实现递归函数，实现不同的处理
    void traversal_subtree(const std::function<void(AttachTreeBaseNode*)>& proc);

    bool update_children() const;
    void set_update_children(bool update_children);

protected:
    void set_transform(const QTransform& transfrom);
    void set_transform_for_expression(const pm::PointE& transfrom);

protected:
    sp::ShapeBase* mp_shape = nullptr;                 // 图形信息

private:
    // trans
    QTransform m_trans;
    //attch point exp
    pm::PointE m_exp_trans = pm::PointE(std::string("0"), std::string("0"));
    QMap<int, QVector<AttachTreeNode*>> m_children;
    bool m_update_children = true;

};
}



#endif // ATTACHTREEBASENODE_H
