#include <QVBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <AttachTreeNodeData.h>

#include "priCoordinateWidget.h"
#include "ShapePointGraphicsItem.h"
#include "AttachTreeUtils.h"
#include "AttachTreeRootNode.h"
#include "AttachTreeNode.h"

namespace pr
{
PrimitiveCoordinateUi::PrimitiveCoordinateUi(at::AttachTreeRootNode *node,  QWidget* parent)
    : QWidget(parent)
    , mp_root_node(node)
{
    QVBoxLayout *layout = new QVBoxLayout();
    mp_table_widget = new QTableWidget(0, 2);
    mp_table_widget->setHorizontalHeaderLabels({tr("X坐标"), tr("Y坐标")});
    mp_table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    mp_table_widget->setSelectionMode(QAbstractItemView::SingleSelection);
    mp_table_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mp_table_widget->horizontalHeader()->setStretchLastSection(true);
    mp_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    TreeNodeData node_data;
    node_data.node_pointer = node;
    node_data.point_idx_in_shape = 0;
    node_data.is_shape = 0;

    //添加新行
    mp_table_widget->insertRow(0);
    QTableWidgetItem *x_item = new QTableWidgetItem(QString::number(0));
    QTableWidgetItem *y_item = new QTableWidgetItem(QString::number(0));
    mp_table_widget->setItem(0, 0, x_item);
    mp_table_widget->setItem(0, 1, y_item);

    //将treenode信息和point_idx添加到x_item的data中
    x_item->setData(Qt::UserRole, QVariant::fromValue(node_data));

    // 选中item connect
    connect(mp_table_widget, &QTableWidget::itemClicked, this, &PrimitiveCoordinateUi::onItemSelectionChanged);

    layout->addWidget(mp_table_widget);
    setLayout(layout);
}

PrimitiveCoordinateUi::~PrimitiveCoordinateUi()
{
}

void PrimitiveCoordinateUi::addCoordToTable(const QVariant &data)
{
    //删除现有行
    int rowCount = mp_table_widget->rowCount();
    if(rowCount > 0)
    {
        mp_table_widget->removeRow(0);
    }

    // QVariant转为treeNodeData
    TreeNodeData node_data = data.value<TreeNodeData>();
    if(nullptr == node_data.node_pointer)
    {
        QMessageBox::critical(this, tr("错误"), "addCoordToTable node_data.node_pointer is nullptr");
        return;
    }

    //获取表达式
    pm::PointE exp_point("0", "0");
    auto tree_node = dynamic_cast<at::AttachTreeNode*>(node_data.node_pointer);
    if(tree_node)
    {
        exp_point = at::AttachTreeUtils::attach_exp_point_coord(tree_node, node_data.point_idx_in_shape, true);
    }

    //添加新行
    mp_table_widget->insertRow(0);
    QTableWidgetItem *x_item = new QTableWidgetItem(exp_point.x().to_str());
    QTableWidgetItem *y_item = new QTableWidgetItem(exp_point.y().to_str());
    mp_table_widget->setItem(0, 0, x_item);
    mp_table_widget->setItem(0, 1, y_item);

    //将treenode信息和point_idx添加到x_item的data中
    x_item->setData(Qt::UserRole, QVariant::fromValue(data));
}

void PrimitiveCoordinateUi::onItemSelectionChanged()
{
    // 获取选中的行
    int row = mp_table_widget->currentRow();

    // 获取选中行中的data数据
    QTableWidgetItem *item = mp_table_widget->item(row, 0);
    if (item)
    {
        // 获取data数据
        TreeNodeData node_data = item->data(Qt::UserRole).value<TreeNodeData>();
        if(nullptr == node_data.node_pointer)
        {
            QMessageBox::critical(this, tr("错误"), "PrimitiveCoordinateUi::onItemSelectionChanged node_data.node_pointer is nullptr");
            return;
        }

        auto attach_tree_node = node_data.node_pointer;

        //如果是根节点直接返回
        if(attach_tree_node == mp_root_node)
        {
            return;
        }
        auto tree_node = dynamic_cast<at::AttachTreeNode*>(attach_tree_node);
        int point_idx = node_data.point_idx_in_shape;
        const auto& point_items = tree_node->point_items();
        sp::ShapePointGraphicsItem* pt_item = point_items[point_idx];

        // 高亮显示选中行对应的节点
        at::AttachTreeUtils::display_single_point_selected(tree_node, point_idx, false);
        sp::ShapePointGraphicsItem::cachedItem = pt_item;
    }
}

void PrimitiveCoordinateUi::onDeleteNode(at::AttachTreeNode* tree_node)
{
    tree_node->traversal_subtree([=](at::AttachTreeBaseNode* node){

        at::AttachTreeNode* sub_node = dynamic_cast<at::AttachTreeNode* >(node);

        for(auto idx = 0; idx < sub_node->point_items().size(); idx++)
        {
            // 该点是否为坐标点，如果是，则删除该点
            if(sub_node->is_coord_point(idx))
            {
                mp_table_widget->removeRow(0);

                // 将根节点设置为坐标点
                mp_root_node->set_coordinate_pos(mp_root_node, idx);

                // 添加新行，将根节点信息填入
                TreeNodeData node_data;
                node_data.node_pointer = mp_root_node;
                node_data.point_idx_in_shape = 0;
                node_data.is_shape = 0;

                mp_table_widget->insertRow(0);
                QTableWidgetItem *x_item = new QTableWidgetItem(QString::number(0));
                QTableWidgetItem *y_item = new QTableWidgetItem(QString::number(0));
                mp_table_widget->setItem(0, 0, x_item);
                mp_table_widget->setItem(0, 1, y_item);

                //将根节点信息和point_idx添加到x_item的data中
                x_item->setData(Qt::UserRole, QVariant::fromValue(node_data));
            }
        }
    });
}

void PrimitiveCoordinateUi::update_table()
{
    // 清空列表
    mp_table_widget->clearContents();
    mp_table_widget->setRowCount(0);

    // 获取坐标点信息
    at::AttachTreeRootNode* root_node = mp_root_node;
    std::pair<at::AttachTreeBaseNode*, int> coord_pos = root_node->get_coordinate_pos();

    TreeNodeData node_data;
    node_data.node_pointer = coord_pos.first;
    node_data.point_idx_in_shape = coord_pos.second;
    node_data.is_shape = 0;

    //获取表达式
    pm::PointE exp_point("0", "0");
    auto tree_node = dynamic_cast<at::AttachTreeNode*>(node_data.node_pointer);
    if(tree_node)
    {
        exp_point = at::AttachTreeUtils::attach_exp_point_coord(tree_node, node_data.point_idx_in_shape, true);
    }

    //添加新行
    mp_table_widget->insertRow(0);
    QTableWidgetItem *x_item = new QTableWidgetItem(exp_point.x().to_str());
    QTableWidgetItem *y_item = new QTableWidgetItem(exp_point.y().to_str());
    mp_table_widget->setItem(0, 0, x_item);
    mp_table_widget->setItem(0, 1, y_item);

    //将treenode信息和point_idx添加到x_item的data中
    x_item->setData(Qt::UserRole, QVariant::fromValue(node_data));
}
}
