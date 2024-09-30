#include <QVBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <AttachTreeNodeData.h>

#include "priAnchorWidget.h"
#include "AttachTreeUtils.h"
#include "ShapePointGraphicsItem.h"
#include "AttachTreeNode.h"
#include "AttachTreeRootNode.h"
#include "primitive.h"

namespace pr
{
PrimitiveAnchorUi::PrimitiveAnchorUi(Primitive* pri, QWidget* parent)
    : QWidget(parent)
    , mp_pri(pri)
{
    QVBoxLayout *layout = new QVBoxLayout();
    mp_table_widget = new QTableWidget(0, 2);
    mp_table_widget->setHorizontalHeaderLabels({tr("X坐标"), tr("Y坐标")});
    mp_table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    mp_table_widget->setSelectionMode(QAbstractItemView::SingleSelection);
    mp_table_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mp_table_widget->horizontalHeader()->setStretchLastSection(true);
    mp_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QHBoxLayout *button_bottom = new QHBoxLayout();
    mp_del_button = new QPushButton(QIcon(":/img/lay_del.png"), tr("删除"));

    // 设置按钮的圆角矩形样式
    QString button_style = "QPushButton {"
                           "border: 2px solid #5F9EA0;"  // 边框颜色
                           "border-radius: 10px;"         // 圆角半径
                           "background-color: #ADD8E6;"   // 背景颜色
                           "padding: 5px 15px;"           // 内边距
                           "}";

    mp_del_button->setStyleSheet(button_style);
    button_bottom->addWidget(mp_del_button);

    // 选中item connect
    connect(mp_table_widget, &QTableWidget::itemClicked, this, &PrimitiveAnchorUi::onItemSelectionChanged);

    connect(mp_del_button, &QPushButton::clicked, this, &PrimitiveAnchorUi::onDeleteItem);

    layout->addWidget(mp_table_widget);
    layout->addLayout(button_bottom);

    setLayout(layout);
}

PrimitiveAnchorUi::~PrimitiveAnchorUi()
{
}

void PrimitiveAnchorUi::addAnchorToTable(const QVariant &data)
{
    // QVariant转为treeNodeData
    TreeNodeData node_data = data.value<TreeNodeData>();
    if(nullptr == node_data.node_pointer)
    {
        QMessageBox::critical(this, tr("错误"), "PrimitiveAnchorUi::addAnchorToTable nodeData.node_pointer is nullptr");
        return;
    }

    //获取表达式
    pm::PointE exp_point("0", "0") ;
    auto tree_node = dynamic_cast<at::AttachTreeNode*>(node_data.node_pointer);
    if(tree_node)
    {
        exp_point = at::AttachTreeUtils::attach_exp_point(tree_node, node_data.point_idx_in_shape, true);
    }

    //将attach point添加到table widget中
    int rowCount = mp_table_widget->rowCount();
    mp_table_widget->insertRow(rowCount);
    QTableWidgetItem *x_item = new QTableWidgetItem(QString("【%1】: %2").arg(exp_point.x().to_double(mp_pri->param_mgr())).arg(exp_point.x().to_str()));
    QTableWidgetItem *y_item = new QTableWidgetItem(QString("【%1】: %2").arg(exp_point.y().to_double(mp_pri->param_mgr())).arg(exp_point.y().to_str()));
    //mp_table_widget->setItem(rowCount, 0, new QTableWidgetItem(QString::number(m_anchor_num++)));
    mp_table_widget->setItem(rowCount, 0, x_item);
    mp_table_widget->setItem(rowCount, 1, y_item);

    //将treenode信息和point_idx添加到x_item的data中
    x_item->setData(Qt::UserRole, QVariant::fromValue(data));

    //将锚点的treenodeid和rowCount对应关系保存起来
    m_anchor_map.insert(QPair<at::AttachTreeNodeId, int>(tree_node->id(), node_data.point_idx_in_shape), x_item);
}

void PrimitiveAnchorUi::onDeleteItem()
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
            QMessageBox::critical(this, tr("错误"), "PrimitiveAnchorUi::deleteItem node_data.node_pointer is nullptr");
            return;
        }

        // 将该附着点设置为非锚点

        auto tree_node = dynamic_cast<at::AttachTreeNode*>(node_data.node_pointer);
        if(tree_node)
        {
            tree_node->set_anchor_point(node_data.point_idx_in_shape, false);

            //删除锚点的treenodeid和rowCount对应关系
            m_anchor_map.remove(QPair<at::AttachTreeNodeId, int>(tree_node->id(), node_data.point_idx_in_shape));
        }
        else
        {
            Q_ASSERT(false);  // HINT@leixunyong。如果程序走到这里来，需要梳理锚点的逻辑。
        }
    }

    // 删除table widget中的行
    mp_table_widget->removeRow(row);
}

void PrimitiveAnchorUi::onItemSelectionChanged()
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
            QMessageBox::critical(this, tr("错误"), "PrimitiveAnchorUi::onItemSelectionChanged node_data.node_pointer is nullptr");
            return;
        }

        at::AttachTreeNode* attach_tree_node = dynamic_cast<at::AttachTreeNode*>(node_data.node_pointer);
        int point_idx = node_data.point_idx_in_shape;
        QVector<sp::ShapePointGraphicsItem*> point_items = attach_tree_node->point_items();
        sp::ShapePointGraphicsItem* pt_item = point_items[point_idx];

        // 高亮显示选中行对应的节点
        at::AttachTreeUtils::display_single_point_selected(attach_tree_node, node_data.point_idx_in_shape, false);
        sp::ShapePointGraphicsItem::cachedItem = pt_item;

    }
}

void PrimitiveAnchorUi::insert_an_item(at::AttachTreeNode* tree_node, int attach_point_idx)
{
    //获取表达式
    pm::PointE exp_point("0", "0") ;
    exp_point = at::AttachTreeUtils::attach_exp_point(tree_node, attach_point_idx, true);

    //将attach point添加到table widget中
    int rowCount = mp_table_widget->rowCount();
    mp_table_widget->insertRow(rowCount);
    QTableWidgetItem *x_item = new QTableWidgetItem(QString("【%1】: %2").arg(exp_point.x().to_double(mp_pri->param_mgr())).arg(exp_point.x().to_str()));
    QTableWidgetItem *y_item = new QTableWidgetItem(QString("【%1】: %2").arg(exp_point.y().to_double(mp_pri->param_mgr())).arg(exp_point.y().to_str()));
    mp_table_widget->setItem(rowCount, 0, x_item);
    mp_table_widget->setItem(rowCount, 1, y_item);

    TreeNodeData data;
    data.node_pointer       = tree_node;
    data.is_shape           = 0;          //is_shape只对treewidget有用，这里填0
    data.point_idx_in_shape = attach_point_idx;

    //将treenode信息和point_idx添加到x_item的data中
    x_item->setData(Qt::UserRole, QVariant::fromValue(data));

    //将锚点的treenodeid和rowCount对应关系保存起来
    m_anchor_map.insert(QPair<at::AttachTreeNodeId, int>(tree_node->id(), data.point_idx_in_shape), x_item);
}

void PrimitiveAnchorUi::clear_table()
{
    // 清空table widget
    mp_table_widget->clearContents();
    mp_table_widget->setRowCount(0);

    // 清空锚点的treenodeid和rowCount对应关系
    m_anchor_map.clear();
}

void PrimitiveAnchorUi::update_table()
{
    // 清空列表
    clear_table();

    // 通过读取信息重建列表
    rebuild_anchor_table(mp_pri->at_root());
}

void PrimitiveAnchorUi::rebuild_anchor_table(at::AttachTreeBaseNode* root_node)
{
    // 使用TreeNode的递归遍历，这里实现对节点的处理
    root_node->traversal_subtree([this](at::AttachTreeBaseNode* node)
    {
        // 根节点不做处理
        if(dynamic_cast<at::AttachTreeRootNode * >(node))
        {
            return;
        }

        // 附着点如果是锚点，插入到列表中
        at::AttachTreeNode* attach_tree_node = dynamic_cast<at::AttachTreeNode*>(node);
        if(nullptr == attach_tree_node)
        {
            return;
        }
        for(int attach_idx = 0; attach_idx < attach_tree_node->point_items().size(); attach_idx++)
        {
            if(attach_tree_node->is_anchor_point(attach_idx))
            {
                insert_an_item(attach_tree_node, attach_idx);
            }
        }
    });
}

// 外部删除节点后触发的槽函数
void PrimitiveAnchorUi::onDeleteNode(at::AttachTreeNode* tree_node)
{
    tree_node->traversal_subtree([=](at::AttachTreeBaseNode* node){
        at::AttachTreeNode* sub_node = dynamic_cast<at::AttachTreeNode* >(node);
        for(auto idx = 0; idx < sub_node->point_items().size(); idx++)
        {
            // 查找该点是否记录在m_anchor_map中，如果是，则删除该点
            auto it = m_anchor_map.find(QPair<at::AttachTreeNodeId, int>(sub_node->id(), idx));
            if (it != m_anchor_map.end())
            {
                // 如果是，则删除该点
                mp_table_widget->removeRow(mp_table_widget->row(it.value()));
                m_anchor_map.erase(it);
            }
        }
    });
}
}
