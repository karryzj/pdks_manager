#ifndef PRIANCHORWIDGET_H
#define PRIANCHORWIDGET_H

//PrimitiveAnchorUi类
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <AttachTreeNode.h>
#include <AttachTreeNodeData.h>
#include <AttachTreeRootNode.h>

namespace pr
{
class Primitive;
class PrimitiveAnchorUi;
class PrimitiveAnchorTable final : public QTableWidget
{
    Q_OBJECT
public:
    PrimitiveAnchorTable(Primitive* pri, PrimitiveAnchorUi *parent);
    ~PrimitiveAnchorTable() override;

signals:
    void rotate_angle_changed(QTableWidgetItem *item);

private:
    void init();
    void deinit();
    bool edit(const QModelIndex &index, QAbstractItemView::EditTrigger trigger, QEvent *event) override;

private slots:
    void on_item_changed(QTableWidgetItem *item);

private:
    Primitive* mp_pri;
    QString m_old_value = "0";
};

class PrimitiveAnchorUi final : public QWidget
{
    Q_OBJECT
public:
    explicit PrimitiveAnchorUi(Primitive* pri,  QWidget *parent);
    ~PrimitiveAnchorUi();

    void save_anchors_to_primitive();
    void load_anchors_from_primitive();

private:
    PrimitiveAnchorTable *mp_table_widget;
    QPushButton *mp_del_button;
    //QVector<TreeNodeData> m_anchor_data; // 锚点数据
    Primitive* mp_pri;

    //存放锚点QPair（tree node id，attach idx）与rowCount的对应关系
    QVector<QPair<sp::ShapePointGraphicsItem*, QTableWidgetItem*>> m_anchor_list;

    // 在列表中插入一条item
    void insert_an_item(at::AttachTreeNode* tree_node, int attach_point_idx);

    bool is_point_item_existed(sp::ShapePointGraphicsItem* point_item);
private slots:
    // 点击删除按钮槽函数
    void on_delete_item();

    // 点击列表item槽函数
    void on_item_selection_changed();

    // 双击修改后，对对应的point的旋转角度进行修改
    void on_rotate_angle_changed(QTableWidgetItem *item);
public slots:

    // 添加锚点到列表槽函数
    void add_anchor_to_table(const QVariant &data);

    // 清除列表
    void clear_table();

    // 刷新列表槽函数
    void update_table();

    // 外部删除节点后触发的槽函数
    void on_delete_node(at::AttachTreeNode* tree_node);
};
}
#endif // PRIANCHORWIDGET_H
