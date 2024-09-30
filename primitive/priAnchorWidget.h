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
class PrimitiveAnchorUi : public QWidget
{
    Q_OBJECT
public:
    explicit PrimitiveAnchorUi(Primitive* pri,  QWidget *parent = nullptr);
    ~PrimitiveAnchorUi();
private:
    QTableWidget *mp_table_widget;
    QPushButton *mp_del_button;
    QVector<TreeNodeData> m_anchor_data; // 锚点数据
    Primitive* mp_pri;

    //存放锚点QPair（tree node id，attach idx）与rowCount的对应关系
    QMap<QPair<int, int>, QTableWidgetItem*> m_anchor_map;

    // 重建锚点列表
    void rebuild_anchor_table(at::AttachTreeBaseNode* root_node);

    // 在列表中插入一条item
    void insert_an_item(at::AttachTreeNode* tree_node, int attach_point_idx);
private slots:
    // 点击删除按钮槽函数
    void onDeleteItem();

    // 点击列表item槽函数
    void onItemSelectionChanged();
public slots:

    // 添加锚点到列表槽函数
    void addAnchorToTable(const QVariant &data);

    // 清除列表
    void clear_table();

    // 刷新列表槽函数
    void update_table();

    // 外部删除节点后触发的槽函数
    void onDeleteNode(at::AttachTreeNode* tree_node);
};
}
#endif // PRIANCHORWIDGET_H
