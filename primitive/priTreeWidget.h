#ifndef PRITREEWIDGET_H
#define PRITREEWIDGET_H

#include <QMetaType>
#include <QTreeWidget>
#include <QVariant>
#include "shapePointGraphicsItem.h"
#include "AttachTreeNode.h"
#include "attachTreeRootNode.h"

namespace pr
{
class PriTreeWidgetMenu;
class PriTreeWidgetItem;
class PrimitiveWindow;
class Primitive;
class PriTreeWidget final : public QTreeWidget
{
    Q_OBJECT
public:
    explicit PriTreeWidget(at::AttachTreeRootNode *node, PrimitiveWindow *parent);
    ~PriTreeWidget();

    void add_tree_node(const at::AttachTreeBaseNode* node, int point_idx,  QTreeWidgetItem* parentItem);
    void visible_shape_points(const at::AttachTreeNode *node);
    void hide_shape_points(const at::AttachTreeNode *node);
    void select_point(const at::AttachTreeNode *node, int point_idx);

    PriTreeWidgetMenu* tree_widge_menu() const;
    at::AttachTreeBaseNode* first_node() const;

    PriTreeWidgetItem* search_tree_widget_item(QGraphicsItem* item);

signals:
    void before_delete_tree_node(at::AttachTreeNode* tree_node);
    void after_delete_tree_node();

public slots:
    void onSceneSelectionChanged(QGraphicsScene* scene);
    void process_tree_widget_context_menu(const QPoint& global_pos, QGraphicsItem* item);
    void update_tree();
private slots:
    void onItemClicked(QTreeWidgetItem* item, int column);
    void onRightButtonClicked(QTreeWidgetItem* item, int column);

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

private:
    QTreeWidgetItem *mp_root_item;
    QTreeWidgetItem *mp_previous_widget_tree_node; //上一个被选中的节点
    at::AttachTreeBaseNode *mp_first_node;
    at::AttachTreeBaseNode *mp_last_node;

    //point到treewidgetitem的映射
    QMap<sp::ShapePointGraphicsItem*, QTreeWidgetItem*> m_point_to_treenode_item_map;

    //shape到treewidgetitem的映射
    QMap<sp::ShapeDrawGraphicsItem*, QTreeWidgetItem*>  m_shape_to_treenode_item_map;

    PriTreeWidgetMenu* mp_tree_widget_menu;

    Primitive* mp_pri;
};
}
#endif // PRITREEWIDGET_H
