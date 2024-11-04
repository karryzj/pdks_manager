#ifndef ATTACHTREENODE_H
#define ATTACHTREENODE_H

#include "attachTreeDefines.h"
#include <AttachTreeBaseNode.h>

namespace sp
{
class ShapeBase;
class ShapeDrawGraphicsItem;
class ShapePointGraphicsItem;
}

namespace at
{
class AttachTreeNodeMgr;

class AT_PUBLIC SetLayerOp : public db::Op
{
public:
    SetLayerOp(ly::LayerInfo *old, ly::LayerInfo *new_layer)
        : db::Op(), m_old(old), m_new(new_layer)
    {}
    ~SetLayerOp() {}

    ly::LayerInfo *old_layer() const
    {
        return m_old;
    }
    ly::LayerInfo *new_layer() const
    {
        return m_new;
    }

private:
    ly::LayerInfo *m_old;
    ly::LayerInfo *m_new;
};

class AT_PUBLIC SetNodeTypeOp : public db::Op
{
public:
    SetNodeTypeOp(const NodeType old, const NodeType new_type)
        : db::Op(), m_old(old), m_new(new_type)
    {}
    ~SetNodeTypeOp() {}

    NodeType old_type() const
    {
        return m_old;
    }
    NodeType new_type() const
    {
        return m_new;
    }

private:
    NodeType m_old;
    NodeType m_new;
};

class AT_PUBLIC SetDirectionOp : public db::Op
{
public:
    SetDirectionOp(const NodeDirection old, const NodeDirection new_dire)
        : db::Op(), m_old(old), m_new(new_dire)
    {}
    ~SetDirectionOp() {}

    NodeDirection old_dire() const
    {
        return m_old;
    }
    NodeDirection new_dire() const
    {
        return m_new;
    }

private:
    NodeDirection m_old;
    NodeDirection m_new;
};



class AT_PUBLIC SetParamsOp : public db::Op
{
public:
    SetParamsOp(const QVector<pm::ParamDecl> &old_params, const QVector<pm::ParamDecl> &new_params)
        : db::Op(), m_old_params(old_params), m_new_params(new_params)
    {}
    ~SetParamsOp() {}

    const QVector<pm::ParamDecl> & old_params() const
    {
        return m_old_params;
    }
    const QVector<pm::ParamDecl> & new_params() const
    {
        return m_new_params;
    }

private:
    QVector<pm::ParamDecl> m_old_params;
    QVector<pm::ParamDecl> m_new_params;
};


class AT_PUBLIC AttachTreeNode final : public AttachTreeBaseNode
{
    Q_OBJECT
    friend class AttachTreePrivateUtils;
private:
    // parent只有当创建AttachTree类型的节点的时候才能为空指针。
    AttachTreeNode(
        AttachTreeBaseNode* parent,
        int parent_attach_point_idx,                            // 为-1的时候，此节点为根节点,
        const QString& shape_name,                              // 图形类型
        const QVector<pm::ParamDecl> & params,                  // 所创建图形的参数
        NodeType node_type,                                     // 控制是辅助定位，还是增加、减去。
        NodeDirection node_dirction,                            // 放在局部坐标系的四个方位的哪一个
        NodeBooleanSubtractType node_boolean_subtra_type,          // delete类型是怎么样的
        ly::LayerInfo* layer,                                   // 图层信息，用于填充
        bool connect_with_mw = false                            // 是否已经和操作界面联动
    );
public:
    ~AttachTreeNode() override;

public:
    virtual pm::ParamMgr* param_mgr() const override;
    virtual QGraphicsView* view() const override;
    virtual AttachTreeNodeMgr* tree_node_mgr() const override;
    virtual void update() override;
    virtual void update_trans() override;

    // void undo (db::Op *op) override;
    // void redo (db::Op *op) override;

public:
    sp::ShapeDrawGraphicsItem* shape_item();
    QVector<sp::ShapePointGraphicsItem*> point_items() const;

    // set/get current node type
    void set_node_type(NodeType node_type);
    NodeType node_type() const;
    // set/get current node direction
    void set_node_direction(NodeDirection node_direction);
    NodeDirection node_direction() const;
    // set/get current node boolean subtract type
    void set_node_boolean_subtract_type(NodeBooleanSubtractType node_direction);
    NodeBooleanSubtractType node_boolean_subtract_type() const;
    // set/get current node layer info
    void set_layer_info(ly::LayerInfo* layer_info);
    ly::LayerInfo* layer_info() const;
    // set/get current node params
    void set_params(const QVector<pm::ParamDecl> & params);
    const QVector<pm::ParamDecl> &params() const;

    // 图形节点
    sp::ShapeBase* shape(void) const;
    const QString& shape_name() const;
    void set_shape_name(const QString& new_shape_name);  // HINT@leixunyong。这里不是简单改了名字，会把shape也改了。

    // 设置/获取附着点是否是锚点
    void set_anchor_point(int attach_point_idx, bool is_anchor);
    bool is_anchor_point(int attach_point_idx) const;

    // 设置/获取附着点是否是坐标点
    void set_coord_point(int attach_point_idx, bool is_coord);
    bool is_coord_point(int attach_point_idx) const;

    int get_parent_attach_point_idx() const;
    AttachTreeBaseNode* parent_node() const;

    void set_new_parent_node(AttachTreeBaseNode* new_parent, int parent_index);

    AttachTreeNodeId id() const;
    void set_new_id();

    bool connect_with_mw() const;
    void set_connect_with_mw(bool connect_with_mw);

public slots:
    void on_param_changed(const QString &key);

private:
    AttachTreeBaseNode* mp_parent;
    int m_parent_attach_point_idx = -1;

    // shape info
    QString m_shape_name;

    // node info
    NodeType m_node_type;
    NodeDirection m_node_direction;
    NodeBooleanSubtractType m_node_bool_subtraction;
    ly::LayerInfo* mp_layer_info;

    AttachTreeNodeId m_tree_node_id = INVALID_ATTACH_TREE_NODE_ID;        // id为INVALID_ATTACH_TREE_NODE_ID时为无效id
    bool m_connect_with_mw;
};
}

#endif // ATTACHTREENODE_H
