#ifndef PRITREEWIDGETMENU_H
#define PRITREEWIDGETMENU_H

#include <QMenu>


class QGraphicsItem;
namespace sp
{
class ShapeDrawGraphicsItem;
}

namespace at
{
class AttachTreeNode;
}

struct RotateCfg;

namespace pr
{
class PriTreeWidgetItem;
class PriTreeWidget;
class Primitive;
class PriTreeWidgetMenu final : public QMenu
{
    Q_OBJECT
public:
    PriTreeWidgetMenu(PriTreeWidget *parent, Primitive* pri);
    ~PriTreeWidgetMenu() override;
public:
    void set_current_pickup_tree_widget_item(PriTreeWidgetItem* tree_widget_item);
    PriTreeWidgetItem* get_current_pickup_tree_widget_item();
    void notify_signal_anchor_action_triggered(const QVariant &data);
    void notify_signal_coord_action_triggered(const QVariant &data);
public slots:
    void record_cut_tree_node_info();
    void record_copied_tree_node_info();
    void paste_cut_or_copied_tree_node_info();
    void set_pickup_tree_widget_item(QGraphicsItem* item);
    void on_before_tree_node(at::AttachTreeNode* tree_node);
private:
    void init();
    void child_rotate(at::AttachTreeNode * root_node, const RotateCfg &rotate_cfg);
    void children_rotate(QVector<at::AttachTreeNode *> children, const RotateCfg &rotate_cfg);
private:
    void showEvent(QShowEvent *event) override;
private slots:
    void onAnchorActionTriggered();
    void onCoordActionTriggered();
    void reedit_shape();
    void onRotateActionTriggered();
    void onMirrorActionTriggered();
    void onConvertShapeActionTriggered();
    void show_shape_info();


signals:
    void signal_anchor_action_triggered(const QVariant &data);
    void signal_coord_action_triggered(const QVariant &data);
private:
    PriTreeWidget* mp_pri_tree_widget;
    Primitive* mp_pri;
    QGraphicsItem* mp_pickup_graphics_item = nullptr;   // HINT@leixunyong。缓存被右键的节点
    at::AttachTreeNode* mp_copied_tree_node = nullptr;
    at::AttachTreeNode* mp_cut_tree_node = nullptr;
};

}


#endif // PRITREEWIDGETMENU_H
