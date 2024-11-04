#include <QVBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <AttachTreeNodeData.h>

#include "priAnchorWidget.h"
#include "AttachTreeUtils.h"
#include "ShapePointGraphicsItem.h"
#include "AttachTreeNode.h"
#include "AttachTreeRootNode.h"
#include "attachTreeNodeMgr.h"
#include "primitive.h"

using namespace pr;

PrimitiveAnchorTable::PrimitiveAnchorTable(Primitive *pri, PrimitiveAnchorUi *parent)
    : QTableWidget(0, 3, parent)
    , mp_pri(pri)
{
    init();
}

PrimitiveAnchorTable::~PrimitiveAnchorTable()
{
    deinit();
}

void PrimitiveAnchorTable::init()
{
    setHorizontalHeaderLabels({tr("X坐标"), tr("Y坐标"), tr("旋转角度")});  // 设置表头
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::DoubleClicked);      // 设置双击编辑触发器
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    bool succeed = connect(this, &QTableWidget::itemChanged, this, &PrimitiveAnchorTable::on_item_changed);
    Q_ASSERT(succeed);
}

void PrimitiveAnchorTable::deinit()
{
    bool succeed = disconnect(this, &QTableWidget::itemChanged, this, &PrimitiveAnchorTable::on_item_changed);
    Q_ASSERT(succeed);
}

bool PrimitiveAnchorTable::edit(const QModelIndex &index, EditTrigger trigger, QEvent *event)
{
    // 只允许第三列的条目可以编辑
    if (index.column() == 2)
    {
        QTableWidgetItem *item = itemFromIndex(index);
        if (item)
        {
            // 记录原始值
            m_old_value = item->text();
        }
        return QTableWidget::edit(index, trigger, event);
    }
    return false;
}

void PrimitiveAnchorTable::on_item_changed(QTableWidgetItem *item)
{
    // 只检查第三列的条目
    if (item->column() == 2)
    {
        QString new_value = item->text();
        bool is_num = pm::Expression::is_num(new_value);
        bool is_exp = pm::Expression::isExpression(mp_pri->param_mgr(), new_value);

        // 进行输入值检查
        if (false == is_num && false == is_exp)
        {
            QMessageBox::warning(this, "Warning!", "Your input is invaild!");
            // 恢复原始值
            item->setText(m_old_value);
        }
        else
        {
            emit rotate_angle_changed(item);
        }
    }
}

PrimitiveAnchorUi::PrimitiveAnchorUi(Primitive* pri, QWidget* parent)
    : QWidget(parent)
    , mp_pri(pri)
{
    QVBoxLayout *layout = new QVBoxLayout();
    mp_table_widget = new PrimitiveAnchorTable(mp_pri, this); // 进行内存托管

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
    connect(mp_table_widget, &QTableWidget::itemClicked, this, &PrimitiveAnchorUi::on_item_selection_changed);
    connect(mp_table_widget, &PrimitiveAnchorTable::rotate_angle_changed, this, &PrimitiveAnchorUi::on_rotate_angle_changed);
    connect(mp_del_button, &QPushButton::clicked, this, &PrimitiveAnchorUi::on_delete_item);

    layout->addWidget(mp_table_widget);
    layout->addLayout(button_bottom);

    setLayout(layout);
}

PrimitiveAnchorUi::~PrimitiveAnchorUi()
{
}

void PrimitiveAnchorUi::save_anchors_to_primitive()
{
    QVector<at::AttachTreeUtils::AttachPointPosInf> anchors;
    for(auto itor = m_anchor_list.begin(); itor != m_anchor_list.end(); itor++)
    {
        auto point_item = itor->first;
        auto tree_node = dynamic_cast<at::AttachTreeNode*>(at::AttachTreeUtils::attach_tree_node_point_item_in(point_item, mp_pri->at_root()));
        int idx = at::AttachTreeUtils::point_item_index_in_tree_node(tree_node, point_item);
        pm::PointE point_ex = at::AttachTreeUtils::attach_exp_point_coord(tree_node, idx, true);
        at::AttachTreeUtils::AttachPointPosInf point_info = {tree_node->id(), idx, point_ex, point_item->rotate_angle()};
        anchors.append(point_info);
    }
    mp_pri->set_json_anchors(anchors);
}

void PrimitiveAnchorUi::load_anchors_from_primitive()
{
    QVector<at::AttachTreeUtils::AttachPointPosInf> anchors = mp_pri->get_json_anchors();
    for(auto itor = anchors.begin(); itor != anchors.end(); itor++)
    {
        auto tree_node = mp_pri->at_root()->tree_node_mgr()->query(itor->id);
        auto idx = itor->attach_point_idx;
        auto is_shape = true;
        tree_node->point_items()[idx]->set_rotate_angle(itor->rotate_angle);
        TreeNodeData node_data{tree_node, idx, is_shape};
        QVariant variant = QVariant::fromValue(node_data);
        add_anchor_to_table(variant);
    }
}

void PrimitiveAnchorUi::add_anchor_to_table(const QVariant &data)
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
    sp::ShapePointGraphicsItem* point_item = nullptr;
    if(tree_node)
    {
        exp_point = at::AttachTreeUtils::attach_exp_point_coord(tree_node, node_data.point_idx_in_shape, true);
        point_item = tree_node->point_items()[node_data.point_idx_in_shape];
    }
    else
    {
        point_item = mp_pri->at_root()->origin_point();
    }

    if(is_point_item_existed(point_item))
    {
        QMessageBox::warning(nullptr, "Warning!", "Current attach point has been anchor point!");
        return;
    }
    //将attach point添加到table widget中
    int rowCount = mp_table_widget->rowCount();
    mp_table_widget->insertRow(rowCount);
    QTableWidgetItem *x_item = new QTableWidgetItem(QString("【%1】: %2").arg(exp_point.x().to_double(mp_pri->param_mgr())).arg(exp_point.x().to_str()));
    QTableWidgetItem *y_item = new QTableWidgetItem(QString("【%1】: %2").arg(exp_point.y().to_double(mp_pri->param_mgr())).arg(exp_point.y().to_str()));
    QTableWidgetItem *rotate_angle_item = new QTableWidgetItem(point_item->rotate_angle());
    //mp_table_widget->setItem(rowCount, 0, new QTableWidgetItem(QString::number(m_anchor_num++)));
    mp_table_widget->setItem(rowCount, 0, x_item);
    mp_table_widget->setItem(rowCount, 1, y_item);
    mp_table_widget->setItem(rowCount, 2, rotate_angle_item);

    //将treenode信息和point_idx添加到x_item的data中
    rotate_angle_item->setData(Qt::UserRole, QVariant::fromValue(data));

    //将锚点的treenodeid和rowCount对应关系保存起来
    QPair<sp::ShapePointGraphicsItem*, QTableWidgetItem*> pair{point_item, rotate_angle_item};
    m_anchor_list.append(pair);
}

void PrimitiveAnchorUi::on_delete_item()
{
    // 获取选中的行
    int row = mp_table_widget->currentRow();

    // 获取选中行中的data数据
    QTableWidgetItem *item = mp_table_widget->item(row, 2);
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
            auto point_item = tree_node->point_items()[node_data.point_idx_in_shape];
            point_item->set_anchor_point(false);
            point_item->set_rotate_angle("0");
            //删除锚点的treenodeid和rowCount对应关系
            for(auto itor = m_anchor_list.begin(); itor < m_anchor_list.end(); itor++)
            {
                if(itor->first == point_item)
                {
                    m_anchor_list.erase(itor);
                    break;
                }
            }
        }
        else
        {
            Q_ASSERT(false);  // HINT@leixunyong。如果程序走到这里来，需要梳理锚点的逻辑。
        }
    }

    // 删除table widget中的行
    mp_table_widget->removeRow(row);
}

void PrimitiveAnchorUi::on_item_selection_changed()
{
    // 获取选中的行
    int row = mp_table_widget->currentRow();

    // 获取选中行中的data数据
    QTableWidgetItem *item = mp_table_widget->item(row, 2);
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

void PrimitiveAnchorUi::on_rotate_angle_changed(QTableWidgetItem *item)
{
    for(auto itor = m_anchor_list.begin(); itor != m_anchor_list.end(); itor++)
    {
        if(itor->second == item)
        {
            itor->first->set_rotate_angle(item->text());
            break;
        }
    }
}

void PrimitiveAnchorUi::insert_an_item(at::AttachTreeNode* tree_node, int attach_point_idx)
{
    //获取表达式
    pm::PointE exp_point("0", "0") ;
    exp_point = at::AttachTreeUtils::attach_exp_point_coord(tree_node, attach_point_idx, true);

    auto point_item = tree_node->point_items()[attach_point_idx];
    if(is_point_item_existed(point_item))
    {
        QMessageBox::warning(nullptr, "Warning!", "Current attach point has been anchor point!");
        return;
    }
    //将attach point添加到table widget中
    int rowCount = mp_table_widget->rowCount();
    mp_table_widget->insertRow(rowCount);
    QTableWidgetItem *x_item = new QTableWidgetItem(QString("【%1】: %2").arg(exp_point.x().to_double(mp_pri->param_mgr())).arg(exp_point.x().to_str()));
    QTableWidgetItem *y_item = new QTableWidgetItem(QString("【%1】: %2").arg(exp_point.y().to_double(mp_pri->param_mgr())).arg(exp_point.y().to_str()));
    QTableWidgetItem *rotate_angle_item = new QTableWidgetItem(point_item->rotate_angle());
    mp_table_widget->setItem(rowCount, 0, x_item);
    mp_table_widget->setItem(rowCount, 1, y_item);
    mp_table_widget->setItem(rowCount, 2, rotate_angle_item);

    TreeNodeData data;
    data.node_pointer       = tree_node;
    data.is_shape           = 0;          //is_shape只对treewidget有用，这里填0
    data.point_idx_in_shape = attach_point_idx;

    //将treenode信息和point_idx添加到x_item的data中
    rotate_angle_item->setData(Qt::UserRole, QVariant::fromValue(data));
    QPair<sp::ShapePointGraphicsItem*, QTableWidgetItem*> pair{point_item, rotate_angle_item};
    //将锚点的treenodeid和rowCount对应关系保存起来
    m_anchor_list.append(pair);
}

bool PrimitiveAnchorUi::is_point_item_existed(sp::ShapePointGraphicsItem *point_item)
{
    for(auto pair : m_anchor_list)
    {
        if(pair.first == point_item)
        {
            return true;
        }
    }
    return false;
}

void PrimitiveAnchorUi::clear_table()
{
    // 清空table widget
    mp_table_widget->clearContents();
    mp_table_widget->setRowCount(0);

    // 清空锚点的treenodeid和rowCount对应关系
    for(auto pair : m_anchor_list)
    {
        pair.first->set_anchor_point(false);
        pair.first->set_rotate_angle("0");
    }
    m_anchor_list.clear();
}

void PrimitiveAnchorUi::update_table()
{
    load_anchors_from_primitive();
}

// 外部删除节点后触发的槽函数
void PrimitiveAnchorUi::on_delete_node(at::AttachTreeNode* tree_node)
{
    tree_node->traversal_subtree([ = ](at::AttachTreeBaseNode* node)
    {
        at::AttachTreeNode* sub_node = dynamic_cast<at::AttachTreeNode* >(node);
        auto point_items = sub_node->point_items();
        for(auto idx = 0; idx < point_items.size(); idx++)
        {
            auto& point_item = point_items[idx];
            // 查找该点是否记录在m_anchor_map中，如果是，则删除该点
            for(auto itor = m_anchor_list.begin(); itor != m_anchor_list.end(); itor++)
            {
                if(itor->first == point_item)
                {
                    mp_table_widget->removeRow(mp_table_widget->row(itor->second));
                    m_anchor_list.erase(itor);
                    break;
                }
            }
        }
    });
}
