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
    Q_ASSERT(new_tree_node->id() == INVALID_ATTACH_TREE_NODE_ID);

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

    update_all();

    emit update_anchor();
}

void pr::PriAttachTreeNodeMgr::set_tree_root_node(at::AttachTreeRootNode *root_node)
{
    mp_root_node = root_node;
}

void PriAttachTreeNodeMgr::on_set_node_type(const at::AttachTreeNodeId&
        tree_node_id)
{
    update_all();
}

void PriAttachTreeNodeMgr::on_set_node_direction(const at::AttachTreeNodeId& tree_node_id)
{
    // 这种情况直接更新进行计算就行
    update_all();
}

void PriAttachTreeNodeMgr::on_set_layer_info(const at::AttachTreeNodeId&
        tree_node_id)
{
    update_all();
}

void PriAttachTreeNodeMgr::on_set_params(const at::AttachTreeNodeId& tree_node_id)
{
    // 这种情况直接更新进行计算就行
    update_all();

    emit update_anchor();
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

        QPointF point = AttachTreeUtils::attach_point(current_tree_node, i, true);

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
    SCOPE_TIMER("update_all");
    static int count = 0;
    count++;
    qDebug() << "第" << count << "次更新";

    // 第一次循环只处理add类型的节点
    for(auto itor = m_dic_node.begin(); itor != m_dic_node.end(); itor++)
    {
        auto current_node_id = itor.key();

        at::AttachTreeNode* current_node = itor.value();
        if(current_node->node_type() == NodeType::ADD)
        {
            QVector<int> delete_ids;
            auto add_tree_node_path = global_painter_path(current_node_id);
            add_tree_node_path = add_tree_node_path.simplified();
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
                delete_tree_node_path = delete_tree_node_path.simplified();
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
