#ifndef ATTACHTREEUTILS_H
#define ATTACHTREEUTILS_H

#include<QGraphicsScene>
#include "pointE.h"
#include "atCommon.h"
#include "attachTreeDefines.h"

namespace sp
{
class ShapePointGraphicsItem;
class ShapeDrawGraphicsItem;
}

namespace at
{
class AttachTreeNode;
class AttachTreeRootNode;
class AttachTreeBaseNode;
class AT_PUBLIC AttachTreeUtils final
{
public:
    struct AttachPointPosInf
    {
        AttachTreeNodeId id;
        int attach_point_idx;
        pm::PointE pos;
    };
    AttachTreeUtils() = delete;
    // HINT@leixunyong。此接口性能比较差，不建议频繁使用。后续性能优化。
    static AttachTreeBaseNode* attach_tree_node_point_item_in(const sp::ShapePointGraphicsItem*point_item, at::AttachTreeRootNode* current_root_node);
    static bool point_item_is_in_tree_node(const AttachTreeNode* attach_tree_node,
                                           const sp::ShapePointGraphicsItem* point_item);
    static int point_item_index_in_tree_node(const AttachTreeBaseNode* attach_tree_node,
            const sp::ShapePointGraphicsItem* point_item);

    static AttachTreeNode* attach_tree_node_shape_item_in(const sp::ShapeDrawGraphicsItem* shape_item, at::AttachTreeRootNode* current_root_node);
    static bool shape_item_is_in_tree_node(const AttachTreeNode* attach_tree_node,
                                           const sp::ShapeDrawGraphicsItem* shape_item);
    static bool a_tree_node_is_b_child_tree_node(const AttachTreeNode* a_tree_node, const AttachTreeNode* b_tree_node);

    // 将一个节点剪切到新的父亲节点上。返回值为true的时候意味着剪切成功。
    static bool cut_tree_node_to_new_parent_node(AttachTreeNode* tree_node, AttachTreeBaseNode* new_parent_node, int new_parent_idx, AttachTreeRootNode* root_node);
    // 将一个节点复制到新的父亲节点上。返回值为true的时候意味着剪切成功。
    static bool copy_tree_node_to_new_parent_node(AttachTreeNode* tree_node, AttachTreeBaseNode* new_parent_node, int new_parent_idx);

    static QPointF parent_attach_point_global_pos(AttachTreeNode* tree_node);
    // static pm::PointE parent_attach_exp_point(AttachTreeNode* tree_node,  bool global);  HINT@leixunyong。此接口暂时不需要实现。

    static QPointF attach_point(AttachTreeNode* tree_node, int attach_point_idx, bool global);
    static pm::PointE attach_exp_point(AttachTreeNode* tree_node, int attach_point_idx, bool global);

    //distance of 2 points
    static double distance(AttachTreeNode* tree_node1, int at_point_idx1, AttachTreeNode* tree_node2 = nullptr, int at_point_idx2 = 0);
    static pm::Expression distance_exp(AttachTreeNode* tree_node1, int at_point_idx1, AttachTreeNode* tree_node2 = nullptr, int at_point_idx2 = 0);
    static pm::Expression x_distance_exp(AttachTreeNode* tree_node1, int at_point_idx1, AttachTreeNode* tree_node2 = nullptr, int at_point_idx2 = 0);
    static pm::Expression y_distance_exp(AttachTreeNode* tree_node1, int at_point_idx1, AttachTreeNode* tree_node2 = nullptr, int at_point_idx2 = 0);

    static at::AttachTreeNode* deep_clone_tree_node(const at::AttachTreeNode* tree_node, at::AttachTreeBaseNode* parent_node, int parent_index);

    // 显示单个附着点
    static void display_single_point(const AttachTreeNode* attach_tree_node,
                                     int point_index);
    // 显示一个图形上的所有附着点
    static void display_points_of_shape(const AttachTreeNode* attach_tree_node);

    // 显示一个选中的附着点
    // block_state为true时会在选择point时阻止信号发射，放置信号成环
    static void display_single_point_selected(const AttachTreeNode* attach_tree_node,
            int point_index,
            bool block_state);
    // 隐藏所有附着点
    static void hide_all_point();

    // 保存主窗口创建的scene的指针信息，在主窗口GraphicScene创建时调用
    static void set_scene(QGraphicsScene* scene);

    // 获取坐标点信息，包括id，idx和位置
    static void get_coordinate_inf(at::AttachTreeRootNode* current_root_node, AttachPointPosInf& point_inf);

    // 设置坐标点信息
    static void set_coordinate_inf(at::AttachTreeRootNode* current_root_node, const AttachPointPosInf& point_inf);

    // 获取锚点信息，包括id，idx和位置
    static void get_anchors_inf(at::AttachTreeRootNode* current_root_node, QVector<AttachPointPosInf>& points_in);

    // 设置锚点信息
    static void set_anchors_inf(at::AttachTreeRootNode* current_root_node, const QVector<AttachPointPosInf>& points_inf);

    // 更新每个node的数据
    static void update_all_nodes_data(at::AttachTreeRootNode* current_root_node);

private:
    // 用于保存主窗口创建时的scene的指针信息
    static QGraphicsScene* mp_scene;

    // 递归的方式找到并设置坐标点，set_coordinate_inf中调用，从根节点开始找
    static void find_coordinate(AttachTreeRootNode* current_root_node,
                                AttachTreeNodeId id,
                                int attach_point_idx);

    // 递归的方式找到并设置锚点，set_anchors_inf中调用
    static void find_anchor(AttachTreeBaseNode* attach_tree_node,
                            AttachTreeNodeId id,
                            int attach_point_idx);

    // 递归的方式获取锚点
    static void get_anchors(AttachTreeBaseNode*   attach_tree_node,
                            QVector<AttachPointPosInf>& points_inf);
};
}
#endif // ATTACHTREEUTILS_H
