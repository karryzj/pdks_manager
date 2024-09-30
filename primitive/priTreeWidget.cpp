#include <QVBoxLayout>
#include <QMessageBox>
#include <QVariant>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <priTreeWidgetItem.h>
#include <QDragEnterEvent>
#include <AttachTreeNodeData.h>
#include <PrimitiveWindow.h>
#include "priTreeWidget.h"
#include "attachTreeNodeMgr.h"
#include "priTreeWidgetMenu.h"
#include "attachTreeUtils.h"
#include "primitive.h"
#include "shapedrawgraphicsitem.h"
#include "primitiveDefines.h"
#include "shapeBase.h"

#define ATTACH_POINTS_NUM 4
using namespace sp;
namespace pr
{
PriTreeWidget::PriTreeWidget(at::AttachTreeRootNode *node, PrimitiveWindow *parent)
    : QTreeWidget(parent)
    , mp_previous_widget_tree_node(nullptr)
    , mp_tree_widget_menu(new PriTreeWidgetMenu(this, parent->current_primitive()))
{
    setHeaderHidden(true);  //隐藏默认显示的"1"

    mp_pri = parent->current_primitive();

    //设置右键菜单
    setContextMenuPolicy(Qt::CustomContextMenu);

    // 连接onItemSelectionChanged信号
    //connect(mp_tree_widget, &QTreeWidget::itemSelectionChanged, this, &PriTreeWidget::onItemSelectionchanged);

    // 连接右键菜单信号
    connect(this, &QTreeWidget::itemPressed, this, &PriTreeWidget::onRightButtonClicked);

    // 注册自定义类型
    qRegisterMetaType<TreeNodeData>("TreeNodeData");

    // 记录根节点
    mp_root_item = new PriTreeWidgetItem(this, mp_pri->at_root() ->origin_point());
    mp_root_item->setText(0, "根节点 (0, 0)");

    mp_first_node = node;
    // 连接信号和槽
    connect(this, &QTreeWidget::itemClicked, this, &PriTreeWidget::onItemClicked); // 添加这一行

    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
}

PriTreeWidget::~PriTreeWidget()
{

}

void PriTreeWidget::update_tree()
{
    clear();

    //增加第一个节点
    mp_root_item = new PriTreeWidgetItem(this, mp_pri->at_root() ->origin_point());
    mp_root_item->setText(0, tr(PRI_ROOT_NODE_NAME));

    //清空上一次选中项目
    mp_previous_widget_tree_node = nullptr;

    TreeNodeData data;
    data.node_pointer = mp_first_node;
    data.point_idx_in_shape = 0;
    data.is_shape = 0;

    // 记录pointData到treeWidget中
    mp_root_item->setData(0, Qt::UserRole, QVariant::fromValue(data));
    add_tree_node(mp_first_node, 0, mp_root_item);
    expandAll();
}

void PriTreeWidget::add_tree_node(const at::AttachTreeBaseNode* node, int point_idx, QTreeWidgetItem* parentItem)
{
    /* node是已经加好树节点的TreeNode。
     * parentItem是node的树的子节点。
     * point_idx是树的子节点的索引。
     * 因此后续需要把node的子节点都逐一加入树中。
     */

    TreeNodeData data;

    const QMap<int, QVector<at::AttachTreeNode*>>& children = node->children();
    // 递归结束条件：如果没有子节点了，就什么都不做
    if(0 == children.size())
    {
        return;
    }

    auto itor = children.find(point_idx);
    if(itor == children.end())
    {
        return;
    }

    const QVector<at::AttachTreeNode*>& child_tree_nodes = itor.value();
    for(auto tree_node : child_tree_nodes)
    {
        // 先增加一个shape的node
        QTreeWidgetItem* shape_widget_item = new PriTreeWidgetItem(parentItem, tree_node->shape_item());
        shape_widget_item->setText(0, tree_node->shape()->shape_name());

        data.node_pointer = tree_node;
        data.point_idx_in_shape = 0;
        data.is_shape = 1;

        //将TreeNode*存储到QTreeWidgetItem中
        shape_widget_item->setData(0, Qt::UserRole, QVariant::fromValue(data));

        // 设置树节点与ShapeDrawGraphicsItem的对应关系
        m_shape_to_treenode_item_map.insert(tree_node->shape_item(), shape_widget_item);

        // 增加shape对应的point
        const QVector<sp::ShapePointGraphicsItem *> point_items = tree_node->point_items();
        for(auto idx = 0; idx < point_items.size(); idx++)
        {
            QTreeWidgetItem* point_widget_item = new PriTreeWidgetItem(shape_widget_item, point_items[idx]);
            point_widget_item->setText(0, point_items[idx]->shape_name());

            // 设置树节点与ShapePointGraphicsItem的对应关系
            m_point_to_treenode_item_map.insert(point_items[idx], point_widget_item);

            //将TreeNode*存储到QTreeWidgetItem中
            data.node_pointer = tree_node;
            data.point_idx_in_shape = idx;
            data.is_shape = 0;
            point_widget_item->setData(0, Qt::UserRole, QVariant::fromValue(data));

            // 处理挂在point下的TreeNode
            add_tree_node(tree_node, idx, point_widget_item);
        }
    }
}

pr::PriTreeWidgetMenu *pr::PriTreeWidget::tree_widge_menu() const
{
    return mp_tree_widget_menu;
}

at::AttachTreeBaseNode *pr::PriTreeWidget::first_node() const
{
    return mp_first_node;
}

pr::PriTreeWidgetItem *pr::PriTreeWidget::search_tree_widget_item(QGraphicsItem *item)
{
    QTreeWidgetItem * tree_widget_graphics_item = nullptr;

    auto shape_item = dynamic_cast<sp::ShapeDrawGraphicsItem*>(item);
    auto point_item = dynamic_cast<sp::ShapePointGraphicsItem*>(item);
    if(shape_item)
    {
        tree_widget_graphics_item = m_shape_to_treenode_item_map[shape_item];
    }
    else if(point_item)
    {
        tree_widget_graphics_item = m_point_to_treenode_item_map[point_item];
    }

    if(nullptr == tree_widget_graphics_item)
    {
        return nullptr;
    }

    return dynamic_cast<PriTreeWidgetItem*>(tree_widget_graphics_item);
}

void PriTreeWidget::onItemClicked(QTreeWidgetItem* item, int column)
{
    PriTreeWidgetItem* pri_item = dynamic_cast<PriTreeWidgetItem*>(item);
    if(pri_item)
    {
        mp_tree_widget_menu->set_current_pickup_tree_widget_item(pri_item);
    }
    //取出TreeNode*
    QVariant variant = item->data(column, Qt::UserRole);
    TreeNodeData tree_node_data = variant.value<TreeNodeData>();
    at::AttachTreeNode *treeNode = dynamic_cast<at::AttachTreeNode *>(tree_node_data.node_pointer);
    if(NULL == treeNode)
    {
        return;
    }
    int point_idx = tree_node_data.point_idx_in_shape;
    int is_shape  = tree_node_data.is_shape;

    // 获取ShapeDrawGraphicsItem，根节点直接返回
    if(NULL == treeNode->shape_item())
    {
        return;
    }

    QVector<ShapePointGraphicsItem*> point_items = treeNode->point_items();
    if(1 == is_shape)
    {
        at::AttachTreeUtils::display_points_of_shape(treeNode);
    }
    else
    {
        // 显示选中点
        ShapePointGraphicsItem* pt_item = point_items[point_idx];
        at::AttachTreeUtils::display_single_point_selected(treeNode, point_idx, true);
        ShapePointGraphicsItem::cachedItem = pt_item;
    }
}

void PriTreeWidget::onRightButtonClicked(QTreeWidgetItem* item, int column)
{
    if(qApp->mouseButtons() == Qt::RightButton) // 只针对鼠标右键
    {
        PriTreeWidgetItem* tree_widget_graphics_item = dynamic_cast<PriTreeWidgetItem*>(item);
        if (tree_widget_graphics_item)
        {
            auto shape_item = dynamic_cast<sp::ShapeDrawGraphicsItem*>(tree_widget_graphics_item->graphics_item());
            auto point_item = dynamic_cast<sp::ShapePointGraphicsItem*>(tree_widget_graphics_item->graphics_item());
            if(shape_item || point_item)
            {
                mp_tree_widget_menu->set_current_pickup_tree_widget_item(tree_widget_graphics_item);

                QRect item_rect = visualItemRect(item);
                QPoint global_pos = viewport()->mapToGlobal(item_rect.topLeft());
                global_pos.rx() += 18;  // HINT@leixunyong。这里的偏移是为了不挡住被右键的QTreeWidgetItem
                global_pos.ry() += 18;
                mp_tree_widget_menu->exec(global_pos);  // 这里只是打开菜单
            }
        }
    }
}

void PriTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->source() == this)
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void pr::PriTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->source() == this)
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void pr::PriTreeWidget::dropEvent(QDropEvent *event)
{
    if (event->source() == this)
    {
        event->setDropAction(Qt::MoveAction);

        auto *source_item = dynamic_cast<PriTreeWidgetItem*>( currentItem());
        auto *target_item = dynamic_cast<PriTreeWidgetItem*>(itemAt(event->pos()));

        if(nullptr == source_item || nullptr == target_item)
        {
            return;
        }

        auto *source_shape_item = dynamic_cast<ShapeDrawGraphicsItem*>(source_item->graphics_item());
        auto *target_point_item = dynamic_cast<ShapePointGraphicsItem*>(target_item->graphics_item());

        if(nullptr == source_shape_item || nullptr == target_point_item)
        {
            return;
        }

        auto tree_node = at::AttachTreeUtils::attach_tree_node_shape_item_in(source_shape_item, mp_pri->at_root());
        auto new_parent_node = at::AttachTreeUtils::attach_tree_node_point_item_in(target_point_item, mp_pri->at_root());
        int new_parent_idx = 0;
        if(dynamic_cast<at::AttachTreeNode * >(new_parent_node))
        {
            new_parent_idx = at::AttachTreeUtils::point_item_index_in_tree_node(dynamic_cast<at::AttachTreeNode*>(new_parent_node), target_point_item);
        }
        Q_ASSERT(-1 != new_parent_idx);
        bool succeed = at::AttachTreeUtils::cut_tree_node_to_new_parent_node(tree_node, new_parent_node, new_parent_idx, mp_pri->at_root());
        if(!succeed)
        {
            return;
        }

        update_tree();
        event->accept();
    }
}

void pr::PriTreeWidget::keyPressEvent(QKeyEvent *event)
{
    QTreeWidget::keyPressEvent(event);
    if (event->key() == Qt::Key_Delete)
    {
        auto tree_widget_item = dynamic_cast<PriTreeWidgetItem*>(  currentItem());
        if(nullptr != tree_widget_item)
        {
            sp::ShapeDrawGraphicsItem* shape_item = dynamic_cast<sp::ShapeDrawGraphicsItem*>(tree_widget_item->graphics_item());
            if(shape_item)
            {
                auto tree_node = at::AttachTreeUtils::attach_tree_node_shape_item_in(shape_item, mp_pri->at_root());
                before_delete_tree_node(tree_node);
                delete tree_node;
                after_delete_tree_node();
            }
        }
    }
}

void PriTreeWidget::onSceneSelectionChanged(QGraphicsScene* scene)
{
    // 通过scene找到选中点
    QList<QGraphicsItem*> selectedItems = scene->selectedItems();
    foreach (QGraphicsItem* item, selectedItems)
    {
        // item如果是点
        if (dynamic_cast<sp::ShapePointGraphicsItem * >(item))
        {
            // 通过point_item获取treeItem
            QTreeWidgetItem* treeItem = m_point_to_treenode_item_map[(ShapePointGraphicsItem*)item];

            // 选中当前treeItem
            setCurrentItem(treeItem);
        }

        // item如果是图形
        else if(dynamic_cast<sp::ShapeDrawGraphicsItem * >(item))
        {
            QTreeWidgetItem* treeItem = m_shape_to_treenode_item_map[(ShapeDrawGraphicsItem*)item];

            // 选中当前treeItem
            setCurrentItem(treeItem);
        }
        else
        {
        }
    }
}

void pr::PriTreeWidget::process_tree_widget_context_menu(const QPoint& global_pos, QGraphicsItem* item)
{
    if (item)
    {
        auto tree_widget_graphics_item = search_tree_widget_item(item);

        if(nullptr == tree_widget_graphics_item)
        {
            return;
        }

        auto point_item = dynamic_cast<ShapePointGraphicsItem*>(item);
        if(point_item)
        {
            point_item->setVisible(true);
        }
        mp_tree_widget_menu->set_current_pickup_tree_widget_item(dynamic_cast<PriTreeWidgetItem*>(tree_widget_graphics_item));
        mp_tree_widget_menu->exec(global_pos);  // 这里只是打开菜单
    }
}

void PriTreeWidgetItem::setup_current_tree_widget_item(QTreeWidgetItem *item, int column)
{
    if(nullptr == item)
    {
        return;
    }

    PriTreeWidget* tree_widget = dynamic_cast<PriTreeWidget*>(treeWidget());
    if(nullptr == tree_widget)
    {
        return;
    }

    tree_widget->tree_widge_menu()->set_current_pickup_tree_widget_item(this);
}

}

