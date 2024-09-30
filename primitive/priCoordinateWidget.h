#ifndef PRICOORDINATEWIDGET_H
#define PRICOORDINATEWIDGET_H

//PrimitiveCoordAnchorUi类，用于显示和操作坐标锚点面板
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>

namespace at
{
class AttachTreeRootNode;
class AttachTreeNode;
}

namespace pr
{
class Primitive;
class PrimitiveCoordinateUi : public QWidget
{
    Q_OBJECT
public:
    explicit PrimitiveCoordinateUi(at::AttachTreeRootNode *node, QWidget *parent = nullptr);
    ~PrimitiveCoordinateUi();
private:
    QTableWidget *mp_table_widget;
    at::AttachTreeRootNode* mp_root_node;
private slots:
    // 点击列表item槽函数
    void onItemSelectionChanged();
public slots:
    void addCoordToTable(const QVariant &data);
    void onDeleteNode(at::AttachTreeNode* tree_node);

    // 刷新列表槽函数
    void update_table();
};
}
#endif // PRICOORDINATEWIDGET_H
