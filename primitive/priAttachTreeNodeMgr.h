#ifndef PRIATTACHTREENODEMGR_H
#define PRIATTACHTREENODEMGR_H

#include "AttachTreeNode.h"
#include "attachTreeNodeMgr.h"
#include <qmap.h>
#include <qset.h>

namespace ly
{
class LayerInfo;
}

namespace cm
{

class Viewport;

}

namespace pr
{
class PriAttachTreeNodeMgr final : public at::AttachTreeNodeMgr
{
    Q_OBJECT
public:
    friend class Primitive;
    void update() override;
    bool is_disable() const override;
    void set_disable(bool is_disable) override;
private:
    PriAttachTreeNodeMgr();
    ~PriAttachTreeNodeMgr() override = default;

private:
    int id_number() override;
    // 注册&反注册
    at::AttachTreeNodeId register_tree_node(at::AttachTreeNode* new_tree_node) override;
    void unregister_tree_node(const at::AttachTreeNodeId& tree_node_id) override;
    void set_tree_root_node(at::AttachTreeRootNode* root_node) override;
    // 设置接口
    void on_set_node_type(const at::AttachTreeNodeId& tree_node_id) override;
    void on_set_node_direction(const at::AttachTreeNodeId& tree_node_id) override;
    void on_set_layer_info(const at::AttachTreeNodeId& tree_node_id) override;
    void on_set_params(const at::AttachTreeNodeId& tree_node_id) override;

    at::AttachTreeNodeId allocate_tree_node_new_id(const at::AttachTreeNodeId& tree_node_id) override;
private:
    cm::Viewport *viewport() override;
    void init_viewport(double dbu) override;
private:
    QPainterPath global_painter_path(const at::AttachTreeNodeId& tree_node_id) const;
    void adjust_point_items_position_and_setup_visible(const at::AttachTreeNodeId& tree_node_id) const;
    bool is_tree_node_in_dic(const at::AttachTreeNodeId& id,
                             QMap<const ly::LayerInfo*, const QSet<at::AttachTreeNodeId>> & dic);
signals:
    void update_anchor();

private slots:
    // HINT@leixunyong。调用此接口前要保证成员变量都已经更新完毕。另外此接口可能有效率问题，需要后续更新。
    void update_all();
private:
    typedef QMap<at::AttachTreeNodeId, at::AttachTreeNode*>        DicTreeNode;
    DicTreeNode m_dic_node;
    at::AttachTreeRootNode* mp_root_node = nullptr;
    cm::Viewport *mp_viewport;

    bool m_disable_update = false;
};
}

#endif // PRIATTACHTREENODEMGR_H
