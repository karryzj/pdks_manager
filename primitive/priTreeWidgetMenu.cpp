#include "priTreeWidgetMenu.h"
#include "AttachTreeNode.h"
#include "AttachTreeUtils.h"
#include "attachTreeNodeMgr.h"
#include "priTreeWidgetItem.h"
#include "primitive.h"
#include "primitiveDefines.h"
#include "priRotateCfgDialog.h"
#include <AttachTreeNodeData.h>
#include <PriTreeWidget.h>
#include <PriUtils.h>
#include <QGraphicsSceneContextMenuEvent>
#include <ShapeDrawGraphicsItem.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include "scopeTimer.h"
#include "shapeBase.h"
#include "expression.h"
using namespace pr;

PriTreeWidgetMenu::PriTreeWidgetMenu(PriTreeWidget *parent, Primitive* pri)
    : QMenu(parent)
    , mp_pri_tree_widget(parent)
    , mp_pri(pri)
{
    init();
}

PriTreeWidgetMenu::~PriTreeWidgetMenu()
{
}

void PriTreeWidgetMenu::set_current_pickup_tree_widget_item(PriTreeWidgetItem *tree_widget_item)
{
    mp_pickup_tree_widget_item = tree_widget_item;
}

PriTreeWidgetItem* PriTreeWidgetMenu::get_current_pickup_tree_widget_item()
{
    return mp_pickup_tree_widget_item;
}

void PriTreeWidgetMenu::notify_signal_anchor_action_triggered(const QVariant &data)
{
    emit signal_anchor_action_triggered(data);
}

void PriTreeWidgetMenu::notify_signal_coord_action_triggered(const QVariant &data)
{
    emit signal_coord_action_triggered(data);
}

void PriTreeWidgetMenu::init()
{
    QAction *cut_action = addAction(PRI_TREE_WIDGET_MENU_CUT_ACTION_NAME);
    cut_action->setObjectName(PRI_TREE_WIDGET_MENU_CUT_ACTION_NAME);
    connect(cut_action, &QAction::triggered, this, &PriTreeWidgetMenu::record_cut_tree_node_info);

    QAction *copy_action = addAction(PRI_TREE_WIDGET_MENU_COPY_ACTION_NAME);
    copy_action->setObjectName(PRI_TREE_WIDGET_MENU_COPY_ACTION_NAME);
    connect(copy_action, &QAction::triggered, this, &PriTreeWidgetMenu::record_copied_tree_node_info);

    QAction *paste_action = addAction(PRI_TREE_WIDGET_MENU_PASTE_ACTION_NAME);
    paste_action->setObjectName(PRI_TREE_WIDGET_MENU_PASTE_ACTION_NAME);
    connect(paste_action, &QAction::triggered, this, &PriTreeWidgetMenu::paste_cut_or_copied_tree_node_info);

    QAction* rotate_action = addAction(PRI_TREE_WIDGET_MENU_ROTATE_ACTION_NAME);
    rotate_action->setObjectName(PRI_TREE_WIDGET_MENU_ROTATE_ACTION_NAME);
    connect(rotate_action, &QAction::triggered, this, &PriTreeWidgetMenu::onRotateActionTriggered);

    QAction* set_anchor_action = addAction(PRI_TREE_WIDGET_MENU_ANCHOR_POINT_ACTION_NAME);
    set_anchor_action->setObjectName(PRI_TREE_WIDGET_MENU_ANCHOR_POINT_ACTION_NAME);
    connect(set_anchor_action, &QAction::triggered, this, &PriTreeWidgetMenu::onAnchorActionTriggered);

    QAction* set_coord_action = addAction(PRI_TREE_WIDGET_MENU_COORD_POINT_ACTION_NAME);
    set_coord_action->setObjectName(PRI_TREE_WIDGET_MENU_COORD_POINT_ACTION_NAME);
    connect(set_coord_action, &QAction::triggered, this, &PriTreeWidgetMenu::onCoordActionTriggered);

    QAction* attribute_action = addAction(PRI_TREE_WIDGET_MENU_ATTRIBUTE_ACTION_NAME);
    attribute_action->setObjectName(PRI_TREE_WIDGET_MENU_ATTRIBUTE_ACTION_NAME);
    connect(attribute_action, &QAction::triggered, this, &PriTreeWidgetMenu::reedit_shape);

    QAction* shape_action = addAction(PRI_TREE_WIDGET_MENU_SHAPE_INFO_ACTION_NAME);
    shape_action->setObjectName(PRI_TREE_WIDGET_MENU_SHAPE_INFO_ACTION_NAME);
    connect(shape_action, &QAction::triggered, this, &PriTreeWidgetMenu::show_shape_info);
}

void PriTreeWidgetMenu::showEvent(QShowEvent *event)
{
    QMenu::showEvent(event);

    QAction* copy_action = findChild<QAction*> (PRI_TREE_WIDGET_MENU_COPY_ACTION_NAME);
    QAction* cut_action = findChild<QAction*> (PRI_TREE_WIDGET_MENU_CUT_ACTION_NAME);
    QAction* paste_action = findChild<QAction*> (PRI_TREE_WIDGET_MENU_PASTE_ACTION_NAME);
    QAction* rotate_action = findChild<QAction*> (PRI_TREE_WIDGET_MENU_ROTATE_ACTION_NAME);
    QAction* set_anchor_action = findChild< QAction*>(PRI_TREE_WIDGET_MENU_ANCHOR_POINT_ACTION_NAME);
    QAction* set_coord_action = findChild< QAction*>(PRI_TREE_WIDGET_MENU_COORD_POINT_ACTION_NAME);
    QAction* attribute_action = findChild< QAction*>(PRI_TREE_WIDGET_MENU_ATTRIBUTE_ACTION_NAME);

    if(nullptr ==  mp_pickup_tree_widget_item)
    {
        setVisible(false);  // 这种情况什么都干不了
        return;
    }

    auto shape_item = dynamic_cast<sp::ShapeDrawGraphicsItem * >(mp_pickup_tree_widget_item->graphics_item());
    auto point_item = dynamic_cast<sp::ShapePointGraphicsItem * >(mp_pickup_tree_widget_item->graphics_item());
    if(shape_item)
    {
        cut_action->setVisible(true);
        copy_action->setVisible(true);
        attribute_action->setVisible(true);
        paste_action->setVisible(false);   // 如果是在对图形进行操作，是不能看到的
        rotate_action->setVisible(true);
    }
    else
    {
        cut_action->setVisible(false);
        copy_action->setVisible(false);
        attribute_action->setVisible(false);
        paste_action->setVisible(true);
        rotate_action->setVisible(false);
    }


    if(mp_copied_tree_node == nullptr && mp_cut_tree_node == nullptr)
    {
        paste_action->setEnabled(false);
    }
    else
    {
        if(point_item)
        {
            paste_action->setEnabled(true);
        }
        else
        {
            paste_action->setEnabled(false);
        }
    }

    if(point_item)
    {
        set_anchor_action->setVisible(true);
        set_coord_action->setVisible(true);
    }
    else
    {
        set_anchor_action->setVisible(false);
        set_coord_action->setVisible(false);
    }
}

void PriTreeWidgetMenu::record_cut_tree_node_info()
{
    if(!mp_pickup_tree_widget_item)
    {
        return;
    }

    auto item = mp_pickup_tree_widget_item->graphics_item();
    sp::ShapeDrawGraphicsItem* shape_item = dynamic_cast<sp::ShapeDrawGraphicsItem * >(item);
    if(!shape_item)
    {
        return;
    }
    mp_cut_tree_node = at::AttachTreeUtils::attach_tree_node_shape_item_in(shape_item, mp_pri->at_root());
    mp_copied_tree_node = nullptr;  // cut和copy在同一个时间点只能有一个生效
}

void PriTreeWidgetMenu::record_copied_tree_node_info()
{
    if(!mp_pickup_tree_widget_item)
    {
        return;
    }

    auto item = mp_pickup_tree_widget_item->graphics_item();
    sp::ShapeDrawGraphicsItem* shape_item = dynamic_cast<sp::ShapeDrawGraphicsItem * >(item);
    if(!shape_item)
    {
        return;
    }
    mp_copied_tree_node = at::AttachTreeUtils::attach_tree_node_shape_item_in(shape_item, mp_pri->at_root());
    mp_cut_tree_node = nullptr;// cut和copy在同一个时间点只能有一个生效
}

void PriTreeWidgetMenu::paste_cut_or_copied_tree_node_info()
{
    if(!mp_pickup_tree_widget_item)
    {
        return;
    }

    if(!mp_cut_tree_node && !mp_copied_tree_node )
    {
        return;
    }

    at::AttachTreeNode* tree_node = nullptr;
    bool is_cut = false;
    if(mp_cut_tree_node && !mp_copied_tree_node)
    {
        tree_node = mp_cut_tree_node;
        is_cut = true;
    }
    else if(!mp_cut_tree_node && mp_copied_tree_node)
    {
        tree_node = mp_copied_tree_node;
        is_cut = false;
    }
    else
    {
        Q_ASSERT(false);
    }

    auto* point_item = dynamic_cast<sp::ShapePointGraphicsItem * >(mp_pickup_tree_widget_item->graphics_item());
    if(!point_item)
    {
        return;
    }

    at::AttachTreeBaseNode* new_parent_tree_node = dynamic_cast<at::AttachTreeNode * >(at::AttachTreeUtils::attach_tree_node_point_item_in(point_item, mp_pri->at_root()));
    int new_parent_idx = -1;

    if(nullptr == new_parent_tree_node)
    {
        new_parent_tree_node = mp_pri->at_root();
        new_parent_idx = 0;
    }
    else
    {
        new_parent_idx = at::AttachTreeUtils::point_item_index_in_tree_node(dynamic_cast<at::AttachTreeNode * >(new_parent_tree_node), point_item);
    }

    Q_ASSERT(new_parent_tree_node);

    Q_ASSERT(new_parent_idx != -1);
    if(is_cut)
    {
        bool succeed = at::AttachTreeUtils::cut_tree_node_to_new_parent_node(tree_node, new_parent_tree_node, new_parent_idx, mp_pri->at_root());
        if(!succeed)
        {
            return;
        }
    }
    else
    {
        bool succeed = at::AttachTreeUtils::copy_tree_node_to_new_parent_node(tree_node, new_parent_tree_node, new_parent_idx);
        if(!succeed)
        {
            return;
        }
    }
    emit mp_pri->signal_add_new_tree();

    mp_pri_tree_widget->update_tree();
    mp_pickup_tree_widget_item = nullptr;

    mp_pri->tree_node_mgr()->update();
}

void PriTreeWidgetMenu::set_pickup_tree_widget_item(QGraphicsItem *item)
{
    mp_pickup_tree_widget_item = mp_pri_tree_widget->search_tree_widget_item(item);
    mp_pri_tree_widget->setCurrentItem(mp_pickup_tree_widget_item);
}

void PriTreeWidgetMenu::onAnchorActionTriggered()
{
    QAction *action = findChild<QAction*>(PRI_TREE_WIDGET_MENU_ANCHOR_POINT_ACTION_NAME);

    if(action == nullptr)
    {
        return;
    }

    // 获取当前选中的项的数据
    QTreeWidgetItem* item = get_current_pickup_tree_widget_item();
    QVariant variant = item->data(0, Qt::UserRole);

    // 判断该点是否已经是锚点，不是才加入到列表中
    TreeNodeData node_data = variant.value<TreeNodeData>();
    if(nullptr == node_data.node_pointer)
    {
        return;
    }

    auto tree_node = dynamic_cast<at::AttachTreeNode*>(node_data.node_pointer);
    if(!tree_node)
    {
        return;
    }

    bool is_anchor = tree_node->is_anchor_point(node_data.point_idx_in_shape);
    if(!is_anchor)
    {
        emit signal_anchor_action_triggered(variant);
        tree_node->set_anchor_point(node_data.point_idx_in_shape, true);
    }
}

void PriTreeWidgetMenu::onCoordActionTriggered()
{
    QAction *action = findChild<QAction*>(PRI_TREE_WIDGET_MENU_COORD_POINT_ACTION_NAME);
    if(action == nullptr)
    {
        return;
    }

    // 获取根节点pointer
    at::AttachTreeRootNode * root_node = dynamic_cast<at::AttachTreeRootNode *>(mp_pri_tree_widget->first_node());

    // 获取当前选中的项的数据
    QTreeWidgetItem* item = get_current_pickup_tree_widget_item();
    QVariant variant = item->data(0, Qt::UserRole);

    // 判断该点是否已经是坐标点，不是才加入到列表中
    TreeNodeData node_data = variant.value<TreeNodeData>();
    if(nullptr == node_data.node_pointer)
    {
        return;
    }

    // 获取坐标点信息
    std::pair<at::AttachTreeBaseNode*, int> coordinate_pos = root_node->get_coordinate_pos();

    // 获取上一个坐标点所在treeNode的point_idx_in_shape
    int previous_point_idx = coordinate_pos.second;

    // 如果该点不是坐标点，则设为坐标点
    if(coordinate_pos.first != node_data.node_pointer
            || coordinate_pos.second != node_data.point_idx_in_shape)
    {
        // 上一个节点如果为根节点，调用根节点中的函数
        if (auto * previous_coord = dynamic_cast<at::AttachTreeRootNode * >(coordinate_pos.first))
        {
            previous_coord->set_coord_point(0, false);
        }
        else if(auto * previous_coord = dynamic_cast<at::AttachTreeNode * >(coordinate_pos.first))
        {
            previous_coord->set_coord_point(previous_point_idx, false);
        }
        else
        {
            return;
        }

        // 将当前点信息发送给坐标点dock处理
        notify_signal_coord_action_triggered(variant);
        auto tree_node = dynamic_cast<at::AttachTreeNode*>(node_data.node_pointer);
        if(!tree_node)
        {
            return;
        }

        // 将当前点标记为坐标点
        tree_node->set_coord_point(node_data.point_idx_in_shape, true);

        // 在根节点记录当前点为坐标点
        root_node->set_coordinate_pos(node_data.node_pointer, node_data.point_idx_in_shape);
    }
}

void PriTreeWidgetMenu::reedit_shape()
{
    if(!mp_pickup_tree_widget_item)
    {
        return;
    }

    auto item = mp_pickup_tree_widget_item->graphics_item();
    sp::ShapeDrawGraphicsItem* shape_item = dynamic_cast<sp::ShapeDrawGraphicsItem * >(item);
    if(!shape_item)
    {
        return;
    }

    emit shape_item->item_double_clicked(shape_item);
}

void PriTreeWidgetMenu::child_rotate(at::AttachTreeNode * root_node, const RotateCfg &rotate_cfg)
{
    auto func = [](at::AttachTreeNode * root_node, const RotateCfg &rotate_cfg)
    {
        QString rotate_degrees;
        rotate_degrees = rotate_cfg.degrees;
        if (!rotate_cfg.by_to)
        {
            auto pre_rotate = root_node->shape()->rotate();
            if (pre_rotate != 0)
            {
                rotate_degrees = rotate_cfg.degrees + "+" + QString::number(pre_rotate);
            }
        }

        auto params = root_node->params();
        for (auto &param : params)
        {
            if (param.key() == "rotate")
            {
                if (pm::Expression::is_expression(rotate_degrees))
                {
                    param.set_expression(rotate_degrees);
                }
                else
                {
                    param.set_expression(rotate_degrees);
                    param.set_value(rotate_degrees.toDouble());
                }
            }
        }
        root_node->set_update_children(false);
        root_node->set_params(params);
        root_node->set_update_children(true);
    };

    func(root_node, rotate_cfg);
    if (rotate_cfg.child_self)
    {
        return;
    }

    QVector<at::AttachTreeNode*> ready_to_update_nodes;
    const auto& mp = root_node->children();
    for(auto itor = mp.begin(); itor != mp.end(); itor++)
    {
        ready_to_update_nodes.append(itor.value());
    }

    while(!ready_to_update_nodes.empty())
    {
        QVector<at::AttachTreeNode*> next_nodes;
        for(auto node : ready_to_update_nodes)
        {
            node->set_update_children(false);
            func(node, rotate_cfg);
            node->set_update_children(true);

            const QMap<int, QVector<at::AttachTreeNode *> > & mp = node->children();
            for(auto itor = mp.begin(); itor != mp.end(); itor++)
            {
                const auto& children = itor.value();
                next_nodes.append(children);
            }
        }
        qSwap(next_nodes, ready_to_update_nodes);
    }
}

void PriTreeWidgetMenu::children_rotate(QVector<at::AttachTreeNode *> nodes, const RotateCfg &rotate_cfg)
{
    for (auto node : nodes)
    {
        child_rotate(node, rotate_cfg);
    }
}

void PriTreeWidgetMenu::onRotateActionTriggered()
{
    QAction *action = findChild<QAction*>(PRI_TREE_WIDGET_MENU_ROTATE_ACTION_NAME);
    if(action == nullptr)
    {
        return;
    }

    if(!mp_pickup_tree_widget_item)
    {
        return;
    }

    auto item = mp_pickup_tree_widget_item->graphics_item();
    sp::ShapeDrawGraphicsItem* shape_item = dynamic_cast<sp::ShapeDrawGraphicsItem * >(item);
    if(!shape_item)
    {
        return;
    }
    auto root_node = at::AttachTreeUtils::attach_tree_node_shape_item_in(shape_item, mp_pri->at_root());

    PriRotateCfgDialog dialog;
    if(dialog.exec() == QDialog::Accepted)
    {
        SCOPE_TIMER("旋转操作用时");
        auto rotate_cfg = dialog.get_result();
        child_rotate(root_node, rotate_cfg);
        mp_pri->tree_node_mgr()->update();
    }

}

void PriTreeWidgetMenu::show_shape_info()
{
    auto item = mp_pickup_tree_widget_item->graphics_item();
    auto shape_item = dynamic_cast<sp::ShapeDrawGraphicsItem*>(item);
    if(nullptr == shape_item)
    {
        return;
    }

    auto tree_node = at::AttachTreeUtils::attach_tree_node_shape_item_in(shape_item, mp_pri->at_root());
    if(nullptr == tree_node)
    {
        return;
    }

    auto node_id = tree_node->id();
    QString str = tree_node->shape_name();
    str += "\n";
    str += (" node id: " + QString::number(node_id));

    QMessageBox::StandardButton resBtn = QMessageBox::information(this, "shape info",
                                         str,
                                         QMessageBox::No | QMessageBox::Yes,
                                         QMessageBox::Yes);

}

void PriTreeWidgetMenu::on_before_tree_node(at::AttachTreeNode *tree_node)
{
    // HINT@leixunyong。如果tree_node是已经被记录的剪切或者复制的节点及其子节点，就重置这些
    if(mp_pickup_tree_widget_item)
    {
        auto item = mp_pickup_tree_widget_item->graphics_item();
        auto shape_item = dynamic_cast<sp::ShapeDrawGraphicsItem*>(item);
        at::AttachTreeNode* recorded_tree_node = nullptr;
        if(shape_item)
        {
            recorded_tree_node = at::AttachTreeUtils::attach_tree_node_shape_item_in(shape_item, mp_pri->at_root());
        }
        else
        {
            auto point_item = dynamic_cast<sp::ShapePointGraphicsItem*>(item);
            auto node = at::AttachTreeUtils::attach_tree_node_point_item_in(point_item, mp_pri->at_root());
            recorded_tree_node = dynamic_cast<at::AttachTreeNode*>(node);
        }

        if(recorded_tree_node && at::AttachTreeUtils::a_tree_node_is_b_child_tree_node(recorded_tree_node, tree_node))
        {
            mp_pickup_tree_widget_item = nullptr;
        }

    }

    if(mp_copied_tree_node && at::AttachTreeUtils::a_tree_node_is_b_child_tree_node(mp_copied_tree_node, tree_node))
    {
        mp_copied_tree_node = nullptr;
    }


    if(mp_cut_tree_node && at::AttachTreeUtils::a_tree_node_is_b_child_tree_node(mp_cut_tree_node, tree_node))
    {
        mp_cut_tree_node = nullptr;
    }
}
