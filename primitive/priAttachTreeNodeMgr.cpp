#include "priAttachTreeNodeMgr.h"
#include "qgraphicsview.h"
#include "scopeTimer.h"
#include "shapeBase.h"

#include <AttachTreeNode.h>
#include <QPainterPath>
#include <layInfo.h>
#include <AttachTreeRootNode.h>
#include <QDebug>
#include <AttachTreeUtils.h>
#include <PriGraphicsScene.h>
#include "shapeDrawBooleanProcessor.h"

#include "viewport.h"

using namespace pr;
using namespace ly;
using namespace at;

PriAttachTreeNodeMgr::PriAttachTreeNodeMgr()
    : at::AttachTreeNodeMgr()
    , mp_viewport(new cm::Viewport())
{

}

int PriAttachTreeNodeMgr::id_number()
{
    if(m_dic_node.isEmpty())
    {
        return 0;
    }

    return m_dic_node.lastKey();
}

at::AttachTreeNodeId PriAttachTreeNodeMgr::register_tree_node(
    at::AttachTreeNode* new_tree_node)
{
    // 检查是否是未处理过的新id
    if (new_tree_node->id() != INVALID_ATTACH_TREE_NODE_ID)
        return new_tree_node->id();

    // 分配新Id
    auto new_tree_node_id = allocate_new_id();
    // 更新主字典
    m_dic_node.insert(new_tree_node_id, new_tree_node);
    return new_tree_node_id;
}

void PriAttachTreeNodeMgr::unregister_tree_node(const at::AttachTreeNodeId&
        tree_node_id)
{
    auto itor = m_dic_node.find(tree_node_id);
    Q_ASSERT(itor != m_dic_node.end());

    // 更新字典
    auto tree_node = m_dic_node[tree_node_id];
    m_dic_node.remove(tree_node_id);

    update();
}

AttachTreeNode *PriAttachTreeNodeMgr::query(const at::AttachTreeNodeId &tree_node_id)
{
    auto itor = m_dic_node.find(tree_node_id);
    if(itor == m_dic_node.end())
    {
        return nullptr;
    }

    return itor.value();
}

void pr::PriAttachTreeNodeMgr::set_tree_root_node(at::AttachTreeRootNode *root_node)
{
    mp_root_node = root_node;
}

AttachTreeNodeId PriAttachTreeNodeMgr::allocate_tree_node_new_id(const at::AttachTreeNodeId &tree_node_id)
{
    auto itor = m_dic_node.find(tree_node_id);
    Q_ASSERT(itor != m_dic_node.end());
    at::AttachTreeNode* tree_node = itor.value();
    at::AttachTreeNodeId new_tree_node_id = allocate_new_id();

    // 移除旧的记录
    m_dic_node.erase(itor);
    // 增加新的记录
    m_dic_node[new_tree_node_id] = tree_node;
    return new_tree_node_id;
}

bool PriAttachTreeNodeMgr::is_tree_node_in_dic(const at::AttachTreeNodeId& id,
        QMap<const ly::LayerInfo*, const QSet<at::AttachTreeNodeId> > & dic)
{
    return false;
}

QPainterPath PriAttachTreeNodeMgr::global_painter_path(const
        at::AttachTreeNodeId& tree_node_id) const
{
    auto current_tree_node = m_dic_node[tree_node_id];
    QTransform apply_trans =
        current_tree_node->transform();  // 这里做了一个拷贝构造

    if (current_tree_node->node_direction() ==
            NodeDirection::TOP_LEFT)
    {
        apply_trans.scale(-1, 1);
    }
    else if (current_tree_node->node_direction() ==
             NodeDirection::BOTTOM_LEFT)
    {
        apply_trans.scale(-1, -1);
    }
    else if (current_tree_node->node_direction() ==
             NodeDirection::BOTTOM_RIGHT)
    {
        apply_trans.scale(1, -1);
    }

    apply_trans.rotate(apply_trans.m11() * apply_trans.m22() * current_tree_node->shape()->rotate());

    QTransform scene_trans = mp_viewport->map_to_scene(apply_trans);

    // 调整Shape的绘制路径的位置
    QPainterPath origin_path = current_tree_node->shape()->painter_path();
    QPainterPath result_path = scene_trans.map(origin_path);
    return result_path;
}

void PriAttachTreeNodeMgr::adjust_point_items_position_and_setup_visible(const at::AttachTreeNodeId& tree_node_id) const
{
    auto current_tree_node = m_dic_node[tree_node_id];

    auto point_items = current_tree_node->point_items();
    for (int i = 0; i < point_items.length(); i++)
    {
        auto point_item = point_items[i];

        QPointF point = AttachTreeUtils::attach_point_coord(current_tree_node, i, true);

        point_item->set_new_position(mp_viewport->map_to_scene(point));
        if(point_item ->shape_item() != nullptr)
        {
            point_item->setVisible(false);
        }
        //point_item->finish_drag();
        point_item->update();
    }
}

void PriAttachTreeNodeMgr::update_all()
{
    if(false)  // 时序排列的delete逻辑
    {
        SCOPE_TIMER("update_all");
        static int count = 0;
        count++;
        qDebug() << "第" << count << "次更新";

        auto view = mp_root_node->view();
        // 获取视图的可视区域
        QRectF view_rect;
        if(view)
        {
            view_rect = mp_root_node->view()->mapToScene(mp_root_node->view()->viewport()->rect()).boundingRect();
        }
        else
        {
            return;
        }
        // 第一次循环只处理add类型的节点
        for(auto itor = m_dic_node.begin(); itor != m_dic_node.end(); itor++)
        {
            auto current_node_id = itor.key();

            at::AttachTreeNode* current_node = itor.value();
            if(current_node->node_type() == NodeType::ADD)
            {
                QVector<int> delete_ids;
                auto add_tree_node_path = global_painter_path(current_node_id);
                QRectF add_tree_node_path_bounding_rect = add_tree_node_path.boundingRect();
                if (!view_rect.intersects(add_tree_node_path_bounding_rect))
                {
                    current_node->shape_item()->set_painter_path(QPainterPath{});  // HINT@leixunyong。不用绘制了。
                    continue;
                }
                QPainterPath delete_nodes_path;
                QVector<QPainterPath> delete_tree_node_paths;
                for(auto next_itor = (itor + 1); next_itor != m_dic_node.end(); next_itor++)
                {
                    auto next_node_id = next_itor.key();
                    at::AttachTreeNode* next_node = next_itor.value();
                    if(next_node->node_type() != NodeType::DELETE)
                    {
                        continue;
                    }

                    if(next_node->layer_info() != current_node->layer_info())
                    {
                        continue;
                    }
                    auto delete_tree_node_path = global_painter_path(next_node_id);
                    QRectF delete_tree_node_path_bounding_rect = delete_tree_node_path.boundingRect();

                    // 判断delete图形是否与add图形有交集
                    if (!add_tree_node_path_bounding_rect.intersects(delete_tree_node_path_bounding_rect))
                    {
                        continue;  // 如果没有交集，直接跳过不处理
                    }

                    delete_tree_node_paths.push_back(delete_tree_node_path);
                    //delete_nodes_path = delete_nodes_path.united(delete_tree_node_path);
                }
                bool user_define_boolean_substracted = true;  // HINT@leixunyong。方便调试。
                if(user_define_boolean_substracted)
                {
                    add_tree_node_path = sp::ShapeDrawBooleanProcessor::boolean_substraction_opera(add_tree_node_path, delete_tree_node_paths);
                }
                else
                {
                    add_tree_node_path = add_tree_node_path.subtracted(delete_nodes_path);
                }
                add_tree_node_path.closeSubpath();
                current_node->shape_item()->set_painter_path(add_tree_node_path);
                current_node->shape_item()->set_paint_type(sp::PaintType::DRAW);
            }
        }

        // 第二次循环只绘制delete和location的节点
        for(auto itor = m_dic_node.begin(); itor != m_dic_node.end(); itor++)
        {
            auto current_node_id = itor.key();
            at::AttachTreeNode* current_node = itor.value();

            if(current_node->node_type() == NodeType::DELETE)
            {
                current_node->shape_item()->set_painter_path(global_painter_path(current_node_id));
                current_node->shape_item()->set_paint_type(sp::PaintType::DASH);
            }
            else if(current_node->node_type() == NodeType::LOCATION)
            {
                auto shape_item = current_node->shape_item();
                shape_item->set_painter_path(global_painter_path(current_node_id));
                shape_item->set_paint_type(sp::PaintType::DRAW);
            }
        }
    }
    else
    {
        // TODO@leixunyong。这里没有进行性能优化，后续再进行。
        QMap<at::AttachTreeNodeId, QPainterPath> dic;
        for(auto itor = m_dic_node.begin(); itor != m_dic_node.end(); itor++)
        {
            auto current_node_id = itor.key();
            auto node_path =  global_painter_path(current_node_id);
            dic[current_node_id] = node_path;
        }

        // 获得距离delete node最近的add类型父亲
        auto delete_nodes_parent = [&](AttachTreeNode* delete_node)
        {
            AttachTreeNode * parent_tree_node = nullptr;

            // 找到最近的add类型的父亲节点
            while(true)
            {
                AttachTreeBaseNode* parent_node = nullptr;
                if(nullptr == parent_tree_node)
                {
                    parent_node = delete_node->parent_node();
                    parent_tree_node = dynamic_cast<AttachTreeNode*>(parent_node);
                }
                else
                {
                    parent_node = parent_tree_node->parent_node();
                    parent_tree_node = dynamic_cast<AttachTreeNode*>(parent_node);
                }

                if(nullptr == parent_tree_node)
                {
                    break;
                }

                if(parent_tree_node->node_type() == NodeType::ADD)
                {
                    break;
                }
                else
                {
                    continue;
                }
            }

            return parent_tree_node;
        };

        // 查找自己的父节点，进行delete
        auto delete_parent = [&](AttachTreeNode* delete_node)
        {
            auto parent_tree_node = delete_nodes_parent(delete_node);
            if(nullptr == parent_tree_node)
            {
                return;
            }

            auto parent_id = parent_tree_node->id();
            auto add_path = dic[parent_id];
            auto delete_path = QVector<QPainterPath> {dic[delete_node->id()]};
            add_path = sp::ShapeDrawBooleanProcessor::boolean_substraction_opera(add_path, delete_path);
            dic[parent_id] = add_path;
            return;
        };

        // 查找父节点的第一层子节点进行delete
        auto delete_brother = [&](AttachTreeNode* delete_node)
        {
            auto parent_tree_node = delete_nodes_parent(delete_node);
            if(nullptr == parent_tree_node)
            {
                return;
            }

            const QMap<int, QVector<at::AttachTreeNode *> > & brothers = parent_tree_node->children();
            for(auto itor = brothers.begin(); itor != brothers.end(); itor++)
            {
                const QVector<at::AttachTreeNode *> & brother_nodes = itor.value();
                for(auto brother_node : brother_nodes)
                {
                    if(brother_node->node_type() == at::NodeType::ADD)
                    {
                        auto add_path = dic[brother_node->id()];
                        auto delete_path = QVector<QPainterPath> {dic[delete_node->id()]};
                        add_path = sp::ShapeDrawBooleanProcessor::boolean_substraction_opera(add_path, delete_path);
                        dic[brother_node->id()] = add_path;
                    }
                }
            }
            return;
        };
        // 查找父节点的全部子节点
        auto delete_children = [&](AttachTreeNode* delete_node)
        {
            auto parent_tree_node = delete_nodes_parent(delete_node);
            if(nullptr == parent_tree_node)
            {
                return;
            }

            const std::function<void(AttachTreeBaseNode*)>& proc = [&](AttachTreeBaseNode* child_node)
            {
                if(parent_tree_node == child_node)
                {
                    return;
                }

                auto child_tree_node = dynamic_cast<AttachTreeNode*>(child_node);
                if(nullptr == child_tree_node)
                {
                    return;
                }

                if(child_tree_node->node_type() == at::NodeType::ADD)
                {
                    auto add_path = dic[child_tree_node->id()];
                    auto delete_path = QVector<QPainterPath> {dic[delete_node->id()]};
                    add_path = sp::ShapeDrawBooleanProcessor::boolean_substraction_opera(add_path, delete_path);
                    dic[child_tree_node->id()] = add_path;
                }
                return;
            };

            parent_tree_node->traversal_subtree(proc);
        };

        for(auto itor = m_dic_node.begin(); itor != m_dic_node.end(); itor++)
        {
            auto current_node_id = itor.key();
            at::AttachTreeNode* current_node = itor.value();
            if(current_node->node_type() == NodeType::DELETE)
            {
                auto boolean_subtract_type = current_node->node_boolean_subtract_type();
                if(boolean_subtract_type == NodeBooleanSubtractType::ONLY_WITH_PARENT_NODE)
                {
                    delete_parent(current_node);
                }
                else if(boolean_subtract_type == NodeBooleanSubtractType::WITH_PARENT_AND_BROTHER_NODE)
                {
                    delete_parent(current_node);
                    delete_brother(current_node);
                }
                else if(boolean_subtract_type == NodeBooleanSubtractType::WITH_PARENT_AND_CHILD_NODE)
                {
                    delete_parent(current_node);
                    delete_children(current_node);
                }

                current_node->shape_item()->set_painter_path(global_painter_path(current_node_id));
                current_node->shape_item()->set_paint_type(sp::PaintType::DASH);
            }
            else if(current_node->node_type() == NodeType::LOCATION)
            {
                auto shape_item = current_node->shape_item();
                shape_item->set_painter_path(global_painter_path(current_node_id));
                shape_item->set_paint_type(sp::PaintType::DRAW);
            }
        }

        // 处理add类型的节点
        for(auto itor = m_dic_node.begin(); itor != m_dic_node.end(); itor++)
        {
            auto node_id = itor.key();
            auto node = itor.value();
            const auto& path = dic[node_id];
            if(node->node_type() == at::NodeType::ADD)
            {
                node->shape_item()->set_painter_path(path);
                node->shape_item()->set_paint_type(sp::PaintType::DRAW);
            }
        }
    }

    // 设置每个点的位置
    for (auto itor = m_dic_node.begin(); itor !=  m_dic_node.end(); itor++)
    {
        adjust_point_items_position_and_setup_visible(itor.key());
        itor.value()->shape_item()->update();
    }

    if(nullptr != mp_root_node)
    {
        mp_root_node->origin_point()->set_new_position(mp_viewport->map_to_scene(QPointF(0, 0)));
        mp_root_node->origin_point()->setVisible(true);
        //mp_root_node->origin_point()->finish_drag();
        mp_root_node->origin_point()->update();
    }
}

cm::Viewport *PriAttachTreeNodeMgr::viewport()
{
    return mp_viewport;
}

void pr::PriAttachTreeNodeMgr::init_viewport(double dbu)
{
    double w = dbu * 1000 * 2;
    double h = w;
    mp_viewport->set_box(QRectF(-w / 2.0, h / 2.0, w, h), true);
}

void PriAttachTreeNodeMgr::update()
{
    if(m_disable_update)
    {
        return;
    }
    update_all();
}

bool PriAttachTreeNodeMgr::is_disable() const
{
    return m_disable_update;
}

void PriAttachTreeNodeMgr::set_disable(bool is_disable)
{
    m_disable_update = is_disable;
}
