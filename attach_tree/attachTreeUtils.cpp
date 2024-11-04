#include "attachTreeUtils.h"
#include "attachTreeNodeMgr.h"
#include "common_defines.h"
#include "shapeDefines.h"
#include "shapeDrawGraphicsItem.h"
#include "shapeBase.h"
#include "attachTreeRootNode.h"
#include "qvector.h"
#include "attachTreeNode.h"
#include "AttachTreePrivateUtils.h"
#include "transformE.h"
#include "viewport.h"

#include "configManager.h"
#include "layerMgr.h"
#include "layerDefines.h"

using namespace at;
using namespace sp;

QGraphicsScene* AttachTreeUtils::mp_scene = nullptr;

AttachTreeBaseNode* AttachTreeUtils::attach_tree_node_point_item_in(const ShapePointGraphicsItem* point_item, at::AttachTreeRootNode* current_root_node)
{
    // 如果它没有对应的shape,说明它是原点
    if(point_item->shape_item() == nullptr)
    {
        return current_root_node;
    }

    QVector<AttachTreeNode*> tree_nodes_need_to_check;
    auto child_items = current_root_node->children();
    for(AttachTreeNode * tree_node : child_items.begin().value())
    {
        tree_nodes_need_to_check.push_back(tree_node);
    }

    while (true)
    {
        if (tree_nodes_need_to_check.isEmpty())
        {
            break;
        }

        QVector<AttachTreeNode*> tree_nodes_in_check = tree_nodes_need_to_check;
        tree_nodes_need_to_check.clear();
        for (auto tree_node : tree_nodes_in_check)
        {
            if (point_item_is_in_tree_node(tree_node, point_item))
            {
                return tree_node;
            }
            else
            {
                const QMap<int, QVector<AttachTreeNode*>> & child_tree_nodes = tree_node->children();
                for (auto itor = child_tree_nodes.begin(); itor != child_tree_nodes.end(); itor++)
                {
                    const QVector<AttachTreeNode*> & child_tree_nodes = itor.value();
                    for (auto child_tree_node : child_tree_nodes)
                    {
                        tree_nodes_need_to_check.push_back(child_tree_node);
                    }
                }
            }
        }
    }

    return current_root_node;
}

bool AttachTreeUtils::point_item_is_in_tree_node(const AttachTreeNode* attach_tree_node,
        const ShapePointGraphicsItem* point_item)
{
    // 仅检查自身的point item
    const auto& point_items = attach_tree_node->point_items();
    for (auto current_point_item : point_items)
    {
        if (current_point_item == point_item)
        {
            return true;
        }
    }
    return false;
}

int AttachTreeUtils::point_item_index_in_tree_node(const AttachTreeBaseNode* attach_tree_node,
        const ShapePointGraphicsItem* point_item)
{
    auto root_node = dynamic_cast<const at::AttachTreeRootNode*>(attach_tree_node);
    if(root_node)
    {
        if(root_node->origin_point() == point_item)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }

    auto child_node = dynamic_cast<const at::AttachTreeNode*>(attach_tree_node);
    if(nullptr == child_node )
    {
        return -1;
    }

    const auto& point_items = child_node->point_items();
    for (int index = 0; index < point_items.size(); index++)
    {
        if (point_items[index] == point_item)
        {
            return index;
        }

    }
    Q_ASSERT(false);
    return -1;
}

AttachTreeNode* AttachTreeUtils::attach_tree_node_shape_item_in(const sp::ShapeDrawGraphicsItem* shape_item, at::AttachTreeRootNode* current_root_node)
{
    QVector<AttachTreeNode*> tree_nodes_need_to_check;

    auto child_items = current_root_node->children();
    for(AttachTreeNode * tree_node : child_items.begin().value())
    {
        tree_nodes_need_to_check.push_back(tree_node);
    }

    while (true)
    {
        if (tree_nodes_need_to_check.isEmpty())
        {
            break;
        }

        QVector<AttachTreeNode*> tree_nodes_in_check = tree_nodes_need_to_check;
        tree_nodes_need_to_check.clear();
        for (auto tree_node : tree_nodes_in_check)
        {
            if (shape_item_is_in_tree_node(tree_node, shape_item))
            {
                return tree_node;
            }
            else
            {

                const QMap<int, QVector<AttachTreeNode*>> & map = tree_node->children();
                for (auto itor = map.begin(); itor != map.end(); itor++)
                {
                    const auto& tree_nodes = itor.value();
                    for (AttachTreeNode * tree_node : tree_nodes)
                    {
                        tree_nodes_need_to_check.push_back(tree_node);
                    }
                }
            }
        }
    }

    return nullptr;
}

bool AttachTreeUtils::shape_item_is_in_tree_node(const AttachTreeNode* attach_tree_node,
        const  sp::ShapeDrawGraphicsItem* shape_item)
{
    sp::ShapeBase* shape = attach_tree_node->shape() ;
    if(!shape)
    {
        return false;
    }

    if (shape->shape_graphics_item() == shape_item)
    {
        return true;
    }
    return false;
}

bool at::AttachTreeUtils::a_tree_node_is_b_child_tree_node(const AttachTreeNode *a_tree_node, const AttachTreeNode *b_tree_node)
{
    Q_ASSERT(a_tree_node && b_tree_node);
    if(a_tree_node == b_tree_node)
    {
        return true;
    }

    const QMap<int, QVector<AttachTreeNode *> > &child_map = b_tree_node->children();
    for(auto itor = child_map.begin(); itor != child_map.end(); itor++)
    {
        const QVector<AttachTreeNode *>& child_nodes = itor.value();
        for(auto child_node : child_nodes)
        {
            if(a_tree_node_is_b_child_tree_node(a_tree_node, child_node))
            {
                return true;
            }
        }
    }

    return false;
}

bool AttachTreeUtils::cut_tree_node_to_new_parent_node(AttachTreeNode *cut_tree_node, AttachTreeBaseNode *new_parent_node, int new_parent_idx, AttachTreeRootNode* root_node)
{
    Q_ASSERT( cut_tree_node && new_parent_node);

    auto new_parent_tree_node = dynamic_cast<at::AttachTreeNode * >( new_parent_node);
    if(new_parent_tree_node)
    {
        // 检查父子关系。如果新的父亲是自己的子，那么禁止进行剪切
        if(at::AttachTreeUtils::a_tree_node_is_b_child_tree_node(dynamic_cast<at::AttachTreeNode * >( new_parent_tree_node), cut_tree_node))
        {
            return false;
        }

        // 断开旧的关系
        auto old_parent_tree_node = cut_tree_node->parent_node();
        old_parent_tree_node->remove_child(cut_tree_node);

        // 建立新的关系
        int idx = new_parent_idx;
        cut_tree_node->set_new_parent_node(new_parent_tree_node, idx);
        new_parent_tree_node->add_child(cut_tree_node, idx);
        // 更新显示
        cut_tree_node->update();
        new_parent_tree_node->update();
        old_parent_tree_node->update();
    }
    else
    {
        // 断开旧的关系
        auto old_parent_tree_node = cut_tree_node->parent_node();
        old_parent_tree_node->remove_child(cut_tree_node);
        // 维护新的关系
        cut_tree_node->set_new_parent_node(root_node, 0);
        root_node->add_child(cut_tree_node, 0);

        // 更新显示
        cut_tree_node->update();
        root_node->update();
        old_parent_tree_node->update();
    }
    return true;
}

bool AttachTreeUtils::copy_tree_node_to_new_parent_node(AttachTreeNode *copy_tree_node, AttachTreeBaseNode *new_parent_node, int new_parent_idx)
{
    Q_ASSERT( copy_tree_node && new_parent_node);

    auto new_parent_tree_node = dynamic_cast<at::AttachTreeNode * >( new_parent_node);
    if(new_parent_tree_node)
    {
        // 检查父子关系。如果新的父亲是自己的子，那么禁止进行复制
        if(at::AttachTreeUtils::a_tree_node_is_b_child_tree_node(dynamic_cast<at::AttachTreeNode * >( new_parent_tree_node), copy_tree_node))
        {
            return false;
        }
    }

    auto new_node = at::AttachTreeUtils::deep_clone_tree_node(copy_tree_node, new_parent_node, new_parent_idx);
    new_node->update();
    return true;
}

QPointF AttachTreeUtils::parent_attach_point_global_pos(AttachTreeNode *tree_node)
{
    sp::ShapeBase* shape = tree_node->shape();
    auto parent_attach_point =  shape->parent_attach_point();
    return tree_node->tree_node_mgr()->viewport()->map_from_scene(parent_attach_point->pos());
}

QPointF AttachTreeUtils::attach_point_coord(AttachTreeNode *tree_node, int attach_point_idx, bool global)
{
    sp::ShapeBase* shape = tree_node->shape();


    auto attach_points = shape->attach_points();
    if(attach_point_idx >= attach_points.size())
    {
        return QPointF{};
    }

    QPointF attach_p = attach_points[attach_point_idx];

    QTransform trans;
    if (tree_node->node_direction() == NodeDirection::TOP_LEFT)
    {
        trans.scale(-1, 1);
    }
    else if (tree_node->node_direction() == NodeDirection::BOTTOM_LEFT)
    {
        trans.scale(-1, -1);
    }
    else if (tree_node->node_direction() == NodeDirection::BOTTOM_RIGHT)
    {
        trans.scale(1, -1);
    }

    trans.rotate(trans.m11() * trans.m22() * tree_node->shape()->rotate());

    attach_p = trans.map(attach_p);

    // local
    if(!global)
    {
        return attach_p;
    }

    // global
    QTransform global_trans = tree_node->transform();
    global_trans.translate(attach_p.x(), attach_p.y());
    return global_trans.map(QPointF(0., 0.));
}

pm::PointE at::AttachTreeUtils::attach_exp_point_coord(AttachTreeNode* tree_node, int attach_point_idx, bool global)
{
    auto shape = tree_node->shape();

    const auto& attach_exp_points = shape->attach_exp_points();
    if(attach_exp_points.size() <= attach_point_idx)
    {
        return pm::PointE{};
    }

    pm::PointE attach_p = attach_exp_points[attach_point_idx];

    pm::TransformE trans;
    if (tree_node->node_direction() == NodeDirection::TOP_LEFT)
    {
        trans.scale(-1, 1);
    }
    else if (tree_node->node_direction() == NodeDirection::BOTTOM_LEFT)
    {
        trans.scale(-1, -1);
    }
    else if (tree_node->node_direction() == NodeDirection::BOTTOM_RIGHT)
    {
        trans.scale(1, -1);
    }

    trans.rotate(trans.m11() * trans.m22() * tree_node->shape()->rotate_exp());

    attach_p.set_trans(trans);

    // local
    if(!global)
    {
        return attach_p;
    }
    // global
    pm::PointE global_exp = tree_node->transorm_for_expression();
    global_exp.translate(attach_p.x(), attach_p.y());
    return global_exp;
}

QString AttachTreeUtils::attach_exp_point_rotate_angle(AttachTreeNode *tree_node, int attach_point_idx)
{
    auto point_items = tree_node->point_items();
    if(point_items.size() <= attach_point_idx)
    {
        return "0";
    }
    return point_items[attach_point_idx]->rotate_angle();
}

QPointF AttachTreeUtils::point_coord(AttachTreeNode *tree_node, const QPointF &point, bool global)
{
    QTransform trans;
    if (tree_node->node_direction() == NodeDirection::TOP_LEFT)
    {
        trans.scale(-1, 1);
    }
    else if (tree_node->node_direction() == NodeDirection::BOTTOM_LEFT)
    {
        trans.scale(-1, -1);
    }
    else if (tree_node->node_direction() == NodeDirection::BOTTOM_RIGHT)
    {
        trans.scale(1, -1);
    }

    trans.rotate(trans.m11() * trans.m22() * tree_node->shape()->rotate());

    auto new_point = trans.map(point);

    // local
    if(!global)
    {
        return new_point;
    }

    // global
    QTransform global_trans = tree_node->transform();
    global_trans.translate(new_point.x(), new_point.y());
    return global_trans.map(QPointF(0., 0.));
}

double AttachTreeUtils::distance(AttachTreeNode *tree_node1, int at_point_idx1, AttachTreeNode *tree_node2, int at_point_idx2)
{
    QPointF p1 = AttachTreeUtils::attach_point_coord(tree_node1, at_point_idx1, true);
    QPointF p2 = AttachTreeUtils::attach_point_coord(tree_node2, at_point_idx2, true);
    double d_x = p1.x() - p2.x();
    double d_y = p1.y() - p2.y();
    return std::sqrt(d_x * d_x + d_y * d_y);
}

pm::Expression AttachTreeUtils::distance_exp(AttachTreeNode *tree_node1, int at_point_idx1, AttachTreeNode *tree_node2, int at_point_idx2)
{
    pm::PointE p1 = AttachTreeUtils::attach_exp_point_coord(tree_node1, at_point_idx1, true);
    pm::PointE p2 = AttachTreeUtils::attach_exp_point_coord(tree_node2, at_point_idx2, true);
    return p1.distance(p2);
}

pm::Expression AttachTreeUtils::x_distance_exp(AttachTreeNode *tree_node1, int at_point_idx1, AttachTreeNode *tree_node2, int at_point_idx2)
{
    pm::PointE p1 = AttachTreeUtils::attach_exp_point_coord(tree_node1, at_point_idx1, true);
    pm::PointE p2 = AttachTreeUtils::attach_exp_point_coord(tree_node2, at_point_idx2, true);
    return p1.x() - p2.x();
}

pm::Expression AttachTreeUtils::y_distance_exp(AttachTreeNode *tree_node1, int at_point_idx1, AttachTreeNode *tree_node2, int at_point_idx2)
{
    pm::PointE p1 = AttachTreeUtils::attach_exp_point_coord(tree_node1, at_point_idx1, true);
    pm::PointE p2 = AttachTreeUtils::attach_exp_point_coord(tree_node2, at_point_idx2, true);
    return p1.y() - p2.y();
}

AttachTreeNode *AttachTreeUtils::deep_clone_tree_node(const at::AttachTreeNode* cloned_tree_node, at::AttachTreeBaseNode* parent_node, int parent_index)
{
    // 首先只管自己这个tree node的数据
    const auto& cloned_shape_name = cloned_tree_node->shape_name();
    const auto& cloned_params = cloned_tree_node->shape()->params();
    auto cloned_node_type = cloned_tree_node->node_type();
    auto cloned_node_direction = cloned_tree_node->node_direction();
    auto cloned_node_boolean_subtract_type = cloned_tree_node->node_boolean_subtract_type();
    ly::LayerInfo* cloned_layer_info = cloned_tree_node->layer_info();

    auto new_node = parent_node->add_child(parent_index, cloned_shape_name, cloned_params, cloned_node_type, cloned_node_direction, cloned_node_boolean_subtract_type, cloned_layer_info);

    // 克隆子对象
    const QMap<int, QVector<at::AttachTreeNode *>>& child_tree_node_map = cloned_tree_node->children();
    for(auto itor = child_tree_node_map.begin(); itor != child_tree_node_map.end(); itor++)
    {
        int attach_index = itor.key();
        const QVector<at::AttachTreeNode *>& child_tree_nodes = itor.value();
        for(auto cloned_child_tree_node : child_tree_nodes)
        {
            auto new_child_node = deep_clone_tree_node(cloned_child_tree_node, new_node, attach_index);
        }
    }
    return new_node;
}

void AttachTreeUtils::hide_all_point()
{
    // 将所有点Item设置为不可见
    QList<QGraphicsItem*> items = mp_scene->items();
    for (QGraphicsItem * item : items)
    {
        sp::ShapePointGraphicsItem* point_item = dynamic_cast<sp::ShapePointGraphicsItem*>(item);
        if (point_item && point_item->shape_item() != nullptr)
        {
            point_item->setVisible(false);
        }
    }
}

// 显示单个附着点
void AttachTreeUtils::display_single_point(const AttachTreeNode* attach_tree_node, int point_index)
{
    // 先隐藏所有附着点
    hide_all_point();

    QVector<sp::ShapePointGraphicsItem*> point_items = attach_tree_node->point_items();

    // 显示单个附着点
    point_items[point_index]->setVisible(true);
}

// 显示一个图形上的所有附着点
void AttachTreeUtils::display_points_of_shape(const AttachTreeNode* attach_tree_node)
{
    // 先隐藏所有附着点
    hide_all_point();

    // 显示选中形状下的所有附着点
    QVector<ShapePointGraphicsItem*> point_items = attach_tree_node->point_items();

    // 清除所有选择状态
    mp_scene->clearSelection();

    for(auto pt_item : point_items)
    {
        pt_item->setVisible(true);
    }
}

// 显示一个选中的附着点
void AttachTreeUtils::display_single_point_selected(const AttachTreeNode* attach_tree_node, int point_index, bool block_state)
{
    // 先隐藏所有附着点
    hide_all_point();

    QVector<sp::ShapePointGraphicsItem*> point_items = attach_tree_node->point_items();

    // block信号防止信号成环
    mp_scene->blockSignals(block_state);

    // 清除所有选择状态
    mp_scene->clearSelection();

    // 显示一个附着点的选中状态
    point_items[point_index]->setVisible(true);
    point_items[point_index]->setSelected(true);
    mp_scene->blockSignals(false);
}

// 保存主窗口创建的scene的指针信息，在主窗口GraphicScene创建时调用
void AttachTreeUtils::set_scene(QGraphicsScene* scene)
{
    AttachTreeUtils::mp_scene = scene;
}

// 获取坐标点信息，包括id，idx和位置表达式
void AttachTreeUtils::get_coordinate_inf(AttachTreeRootNode* current_root_node, AttachPointPosInf& point_inf)
{
    std::pair<at::AttachTreeBaseNode*, int> coordinate = current_root_node->get_coordinate_pos();

    point_inf.attach_point_idx = coordinate.second;

    if(dynamic_cast<at::AttachTreeRootNode * >(coordinate.first))
    {
        // 根节点为id为
        point_inf.id = 0;
        point_inf.pos = pm::PointE("0", "0") ;
    }
    else if(auto tree_node = dynamic_cast<at::AttachTreeNode * >(coordinate.first))
    {
        // 获取非根节点的id
        point_inf.id = tree_node->id();
        point_inf.pos = at::AttachTreeUtils::attach_exp_point_coord(tree_node, point_inf.attach_point_idx, true);
    }
    else
    {
        point_inf.id  = -1;
        point_inf.pos = pm::PointE("0", "0") ;
    }
}

// 设置坐标点信息
void AttachTreeUtils::set_coordinate_inf(AttachTreeRootNode* current_root_node, const AttachPointPosInf& point_inf)
{
    if(point_inf.id < 0)
    {
        return;
    }
    else if(0 == point_inf.id)
    {
        // id为0是根节点
        current_root_node->set_coord_point(0, true);
        current_root_node->set_coordinate_pos(current_root_node, 0);
        return;
    }

    // 不是根节点先将根节点设置为非坐标点
    current_root_node->set_coord_point(0, false);
    find_coordinate(current_root_node, point_inf.id, point_inf.attach_point_idx);
}

void AttachTreeUtils::find_coordinate(AttachTreeRootNode* root_node, AttachTreeNodeId id, int attach_point_idx)
{
    // 使用TreeNode的递归遍历，这里实现对节点的处理
    root_node->traversal_subtree([ = ](at::AttachTreeBaseNode* node)
    {
        // 根节点不做处理
        if(dynamic_cast<at::AttachTreeRootNode * >(node))
        {
            return;
        }

        at::AttachTreeNode* sub_node = dynamic_cast<at::AttachTreeNode* >(node);
        for(int attach_idx = 0; attach_idx < sub_node->point_items().size(); attach_idx++)
        {
            if(id == sub_node->id() && attach_point_idx == attach_idx)
            {
                sub_node->set_coord_point(attach_idx, true);
                root_node->set_coordinate_pos(sub_node, attach_idx);
                return;
            }
        }
    });
}

// void AttachTreeUtils::set_anchors_inf( at::AttachTreeRootNode* current_root_node, const QVector<AttachPointPosInf>& points_inf)
// {
//     // 根据json中读出的锚点信息，查找到对应的treeNode节点并设置
//     for(const AttachPointPosInf &posInf : points_inf)
//     {
//         set_tree_node_anchor_point(current_root_node, posInf);
//     }
// }

void at::AttachTreeUtils::update_all_nodes_data(AttachTreeRootNode *current_root_node)
{
    QVector<AttachTreeBaseNode*> ready_to_update_nodes = {current_root_node};
    while(!ready_to_update_nodes.empty())
    {
        QVector<AttachTreeBaseNode*> next_nodes;
        for(auto node : ready_to_update_nodes)
        {
            node->set_update_children(false);
            node->update();
            node->set_update_children(true);

            const QMap<int, QVector<at::AttachTreeNode *> > & mp = node->children();
            for(auto itor = mp.begin(); itor != mp.end(); itor++)
            {
                const auto& children = itor.value();
                for(auto& child : children)
                {
                    next_nodes.append(child);
                }
            }
        }
        qSwap(next_nodes, ready_to_update_nodes);
    }
    return;
}

AttachTreeNode *AttachTreeUtils::create_inner_rounded_corner(sp::ShapePointGraphicsItem *point_item, AttachTreeRootNode* root_node, ly::LayerMgr* ly_mgr)
{
    AttachTreeBaseNode* node = AttachTreeUtils::attach_tree_node_point_item_in(point_item, root_node);
    AttachTreeNode* tree_node = dynamic_cast< AttachTreeNode*>(node);
    if(nullptr == tree_node)
    {
        return nullptr;
    }
    // 只能处理矩形的四个点和三角形的序号为1的点
    int idx = AttachTreeUtils::point_item_index_in_tree_node(tree_node, point_item);
    const auto& shape_name = tree_node->shape_name();

    if(tree_node->node_type() != NodeType::ADD)
    {
        return nullptr;
    }

    if( (shape_name != SHAPE_RECTANGLE) && (shape_name != SHAPE_TRIANGLE))
    {
        return nullptr;
    }

    if( shape_name == SHAPE_TRIANGLE && idx != 1)
    {
        return nullptr;
    }

    // 读取配置中的内切矩形的尺寸
    const QVariant & inner_corner_radius_var = cm::ConfigManager::instance()->query(CM_INNER_CORNER_RADIUS_KEY);
    QString inner_corner_radius_str = inner_corner_radius_var.toString();
    auto param_mgr = root_node->param_mgr();
    bool is_num = pm::Expression::is_num(inner_corner_radius_str);
    bool is_exp = pm::Expression::isExpression(param_mgr, inner_corner_radius_str);
    if(false == is_num && false == is_exp)
    {
        return nullptr;
    }

    auto location_layer =  ly_mgr->get_layer_info_by_name(LAYER_LOCATION_LAYER_NAME);
    if(nullptr == location_layer)
    {
        return nullptr;
    }

    // 创建一个delete的矩形
    QVector<pm::ParamDecl> rectangle_params;
    {
        pm::ParamDecl width(SHAPE_RECTANGLE_WIDTH, inner_corner_radius_str);
        pm::ParamDecl height(SHAPE_RECTANGLE_HEIGHT, inner_corner_radius_str);
        pm::ParamDecl rotate(SHAPE_ROTATE, 0);
        rectangle_params.append(width);
        rectangle_params.append(height);
        rectangle_params.append(rotate);
    }

    auto delete_rectangle_node = tree_node->add_child(idx, SHAPE_RECTANGLE, rectangle_params, NodeType::DELETE, NodeDirection::BOTTOM_LEFT, NodeBooleanSubtractType::ONLY_WITH_PARENT_NODE, location_layer);
    if(nullptr == delete_rectangle_node)
    {
        return nullptr;
    }

    // 在delete_rectangle_node的1位置再加一个add的扇形
    QVector<pm::ParamDecl> sector_params;
    {
        pm::ParamDecl sector_inside_radius(SHAPE_SECTOR_INSIDE_RADIUS, 0);
        pm::ParamDecl sector_outside_radius(SHAPE_SECTOR_OUTSIDE_RADIUS, inner_corner_radius_str);
        pm::ParamDecl sector_start_angle(SHAPE_SECTOR_START_ANGLE, 180);
        pm::ParamDecl sector_end_angle(SHAPE_SECTOR_END_ANGLE, 270);
        pm::ParamDecl sector_rotate_angle(SHAPE_ROTATE, 0);
        sector_params.append(sector_inside_radius);
        sector_params.append(sector_outside_radius);
        sector_params.append(sector_start_angle);
        sector_params.append(sector_end_angle);
        sector_params.append(sector_rotate_angle);
    }
    auto add_sector_node = delete_rectangle_node->add_child(1, SHAPE_SECTOR, sector_params, NodeType::ADD, NodeDirection::BOTTOM_LEFT, NodeBooleanSubtractType::NONE, tree_node->layer_info());
    if(nullptr == add_sector_node)
    {
        return nullptr;
    }

    return delete_rectangle_node;
}

AttachTreeNode *AttachTreeUtils::create_outer_rounded_corner(sp::ShapePointGraphicsItem *point_item, AttachTreeRootNode* root_node, ly::LayerMgr* ly_mgr)
{
    AttachTreeBaseNode* node = AttachTreeUtils::attach_tree_node_point_item_in(point_item, root_node);
    AttachTreeNode* tree_node = dynamic_cast< AttachTreeNode*>(node);
    if(nullptr == tree_node)
    {
        return nullptr;
    }
    // 只能处理矩形的四个点和三角形的序号为1的点
    int idx = AttachTreeUtils::point_item_index_in_tree_node(tree_node, point_item);
    const auto& shape_name = tree_node->shape_name();

    if(tree_node->node_type() != NodeType::ADD)
    {
        return nullptr;
    }

    if( (shape_name != SHAPE_RECTANGLE) && (shape_name != SHAPE_TRIANGLE))
    {
        return nullptr;
    }

    if( shape_name == SHAPE_TRIANGLE && idx != 1)
    {
        return nullptr;
    }

    // 读取配置中的内切矩形的尺寸
    const QVariant & inner_corner_radius_var = cm::ConfigManager::instance()->query(CM_INNER_CORNER_RADIUS_KEY);
    QString inner_corner_radius_str = inner_corner_radius_var.toString();
    auto param_mgr = root_node->param_mgr();
    bool is_num = pm::Expression::is_num(inner_corner_radius_str);
    bool is_exp = pm::Expression::isExpression(param_mgr, inner_corner_radius_str);
    if(false == is_num && false == is_exp)
    {
        return nullptr;
    }

    auto location_layer =  ly_mgr->get_layer_info_by_name(LAYER_LOCATION_LAYER_NAME);
    if(nullptr == location_layer)
    {
        return nullptr;
    }

    // 创建一个add的矩形
    QVector<pm::ParamDecl> rectangle_params;
    {
        pm::ParamDecl width(SHAPE_RECTANGLE_WIDTH, inner_corner_radius_str);
        pm::ParamDecl height(SHAPE_RECTANGLE_HEIGHT, inner_corner_radius_str);
        pm::ParamDecl rotate(SHAPE_ROTATE, 0);
        rectangle_params.append(width);
        rectangle_params.append(height);
        rectangle_params.append(rotate);
    }

    auto add_rectangle_node = tree_node->add_child(idx, SHAPE_RECTANGLE, rectangle_params, NodeType::ADD, NodeDirection::TOP_LEFT, NodeBooleanSubtractType::NONE, tree_node->layer_info());
    if(nullptr == add_rectangle_node)
    {
        return nullptr;
    }

    // 在add_rectangle_node的1位置再加一个delete的扇形
    QVector<pm::ParamDecl> sector_params;
    {
        pm::ParamDecl sector_inside_radius(SHAPE_SECTOR_INSIDE_RADIUS, 0);
        pm::ParamDecl sector_outside_radius(SHAPE_SECTOR_OUTSIDE_RADIUS, inner_corner_radius_str);
        pm::ParamDecl sector_start_angle(SHAPE_SECTOR_START_ANGLE, 180);
        pm::ParamDecl sector_end_angle(SHAPE_SECTOR_END_ANGLE, 270);
        pm::ParamDecl sector_rotate_angle(SHAPE_ROTATE, 0);
        sector_params.append(sector_inside_radius);
        sector_params.append(sector_outside_radius);
        sector_params.append(sector_start_angle);
        sector_params.append(sector_end_angle);
        sector_params.append(sector_rotate_angle);
    }
    auto delete_sector_node = add_rectangle_node->add_child(1, SHAPE_SECTOR, sector_params, NodeType::DELETE, NodeDirection::TOP_LEFT, NodeBooleanSubtractType::ONLY_WITH_PARENT_NODE, location_layer);
    if(nullptr == delete_sector_node)
    {
        return nullptr;
    }

    return add_rectangle_node;
}

void AttachTreeUtils::rotate_rounded_corner(AttachTreeNode *parent_node, int point_item_idx_in_parent, AttachTreeNode *rounded_node)
{
    auto parent_node_direction = parent_node->node_direction();
    qreal basic_angle = 0;
    qreal delat_angle = 0;
    if(NodeDirection::TOP_LEFT == parent_node_direction)
    {
        basic_angle = 0;
        delat_angle = -90;
    }
    else if(NodeDirection::TOP_RIGHT == parent_node_direction)
    {
        basic_angle = 90;
        delat_angle = 90;
    }
    else if(NodeDirection::BOTTOM_LEFT == parent_node_direction)
    {
        basic_angle = -90;
        delat_angle = 90;
    }
    else if(NodeDirection::BOTTOM_RIGHT == parent_node_direction)
    {
        basic_angle = -180;
        delat_angle = -90;
    }

    qreal rotate_angle = basic_angle + point_item_idx_in_parent*delat_angle;
    QString rotate_angle_str = QString::number(rotate_angle);
    const auto& params = parent_node->params();
    QString parent_angle = 0;
    for(const auto& param : params)
    {
        if(param.key() == SHAPE_ROTATE)
        {
            parent_angle = param.expression();
            break;
        }
    }

    pm::Expression rounded_node_rotate_angle = pm::Expression(parent_angle) + pm::Expression(rotate_angle);
    std::function<void(AttachTreeBaseNode*)> proc = [rounded_node_rotate_angle](AttachTreeBaseNode* node)
    {
        auto tree_node = dynamic_cast<AttachTreeNode*>(node);
        if(nullptr == tree_node)
        {
            return;
        }

        auto params = tree_node->params();
        for(auto& param : params)
        {
            if(param.key() == SHAPE_ROTATE)
            {
                qreal value = rounded_node_rotate_angle.to_double(tree_node->param_mgr());
                param.set_value(value);
                param.set_expression(rounded_node_rotate_angle.to_str());
                break;
            }
        }
        tree_node->set_params(params);
    };
    rounded_node->traversal_subtree(proc);
}

// void AttachTreeUtils::set_tree_node_anchor_point(AttachTreeBaseNode* node, const AttachPointPosInf& point_info)
// {
//     auto tree_node = node->tree_node_mgr()->query(point_info.id);
//     Q_ASSERT(nullptr != tree_node);
//     auto point_items = tree_node->point_items();
//     Q_ASSERT(point_items.size() > point_info.attach_point_idx);
//     auto point_item = point_items[point_info.attach_point_idx];
//     Q_ASSERT(nullptr != point_item);
//     point_item->set_anchor_point(true);
//     point_item->set_rotate_angle(point_info.rotate_angle);
// }
