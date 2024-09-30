#ifndef ATTACHTREEROOTNODE_H
#define ATTACHTREEROOTNODE_H

#include <AttachTreeBaseNode.h>
#include <QSet>

namespace sp
{
class ShapePointGraphicsItem;
}

namespace at
{
// AttachTree是个特殊的“起始”TreeNode，它天生就具有一个起始附着点(0.0,0.0)
class AT_PUBLIC AttachTreeRootNode final : public AttachTreeBaseNode
{
    Q_OBJECT
public:
    AttachTreeRootNode(pm::ParamMgr *param_mgr, QGraphicsView* view, AttachTreeNodeMgr* tree_node_mgr);
    ~AttachTreeRootNode() override;

public:
    virtual pm::ParamMgr* param_mgr() const override;
    virtual QGraphicsView* view() const override;
    virtual AttachTreeNodeMgr* tree_node_mgr() const override;
    virtual void update() override;
    virtual void update_trans() override;

public:
    // 设置坐标点
    void set_coordinate_pos(AttachTreeBaseNode* node, int point_idx);

    // 获取坐标点
    std::pair<AttachTreeBaseNode*, int> get_coordinate_pos() const;
    sp::ShapePointGraphicsItem* origin_point() const;

    void set_new_graphics_view(QGraphicsView* view);

    // 获取原点是否为坐标点，这里实际原点只有1个点，attach_point_idx是无用参数，为了与普通节点接口保持一致加了这个参数
    void set_coord_point(int attach_point_idx, bool is_coord);
    bool is_coord_point(int attach_point_idx) const;

private:
    pm::ParamMgr* mp_param_mgr;
    QGraphicsView* mp_view = nullptr;
    AttachTreeNodeMgr* mp_tree_node_mgr;

    std::pair<at::AttachTreeBaseNode*, int> m_coordinate_pos;
    sp::ShapePointGraphicsItem* mp_origin_point;

    bool m_is_coord;

};
}

#endif // ATTACHTREEROOTNODE_H
