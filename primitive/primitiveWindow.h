#ifndef PRIMITIVEWINDOW_H
#define PRIMITIVEWINDOW_H

#include "attachTreeDefines.h"
#include "paramMgr.h"
#include <QObject>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QMap>
#include <QVBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QGraphicsPolygonItem>
#include <QUndoStack>

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

class PriDrawShapeAction;

namespace at
{
class AttachTreeRootNode;
}

namespace ly
{
class LayerInfo;
class LayerWidget;
}

namespace sp
{
class ShapeDrawGraphicsItem;
class ShapePointGraphicsItem;
}

namespace at
{
class AttachTreeNode;
}

namespace pm
{
class ParamEditWidget;
class ParamRuleWidget;
}
namespace pr
{

class PriGraphicsView;
class PriGraphicsScene;
class PriTreeWidget;
class Primitive;
class PrimitiveAnchorUi;
class PrimitiveCoordinateUi;
class PriRulerDialog;
class PriDockWidget;
class PrimitiveWindow final: public QMainWindow
{
    Q_OBJECT
public:
    PrimitiveWindow(QWidget* parent = nullptr, QString pri_name = "default" /*图元名称**/,
                    bool new_create = true/*是否是新建，false-加载*/);

    ~PrimitiveWindow() override;

    void fill_data();
    void fill_tree_node();
    void fill_view();

    Primitive* current_primitive() const;
protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    void init_general_actions(void);
    void init_draw_shape_actions(void);
    void init_align_actions(void);
    void init_function_actions(void);
    void init_keyboard_shortcuts(void);

    void set_connects();
    void remove_connects();

    void connect_new_tree_node(at::AttachTreeNode* new_tree_node);

private:
    at::AttachTreeNode* add_new_tree_node(const QString& shape_name,
                                          const QVector<pm::ParamDecl> & params,
                                          at::NodeType node_type,
                                          at::NodeDirection node_direction,
                                          at::NodeBooleanSubtractType node_boolean_subtract_type,
                                          ly::LayerInfo* layer_info);
    void save_img();
    void fit_view();
    void restore_view();
    void on_view_changed();

    void match_viewport();

private slots:
    void newly_draw_shape_dialog();
    void update_mouse_position(const QPointF& scenePos);
    void edit_draw_shape_dialog(sp::ShapeDrawGraphicsItem* item);
    void save();
    void adjust_mouse_position(const QPointF& scenePos);
    void save_json();
    void load_json();
    void after_delete_tree_node();
    void on_scene_selection_changed();
    void add_dragged_rectangle_info(sp::ShapePointGraphicsItem* point_item,
                                    QPointF end_point);
    void update_current_primitive();
    void delete_tree_nodes_with_layer_info(ly::LayerInfo* layer_info);
    void record_mouse_left_button_click_point(const QPoint& pos);
    void align_points();
    void rule_2_points_distance();
    void setup_current_grid_resolution();
    void cancel_current_operations();
    void finish_pickup_polygen_point();
    void connect_tree_node_with_mw();
    void redo();
    void undo();
    void set_arc_length();
    void set_rounded_corner_params();

    void add_inner_rounded_corner_for_shape();
    void add_outer_rounded_corner_for_shape();

    void on_dock_widget_resize();
signals:
    // scene选择改变之后，发送给treewidget的信号
    void scene_selection_changed_to_tree(QGraphicsScene* scene);
    void primitive_wnd_closed();

signals:
    // 删除锚点信号
    void scene_delete_node_to_anchor_widget(int node_id, int point_idx_in_shape);

    void align_operation(const QString& align_type);
private:
    QPointF current_scene_pos() const;

    void clear_cache();
private:
    QMap<QString, PriDrawShapeAction*> m_draw_shape_actions;
    pm::ParamMgr*  mp_param_mgr;
    PriGraphicsScene* mp_graphics_scene;
    PriGraphicsView* mp_graphics_view;

    PriDockWidget* mp_dock_widget_top;
    PriDockWidget* mp_dock_widget_bottom;
    PriDockWidget* mp_param_list_dock;
    PriDockWidget* mp_param_constraints_dock;
    PriDockWidget* mp_coord_anchor_dock;
    PriDockWidget* mp_anchor_dock;

    PriTreeWidget* mp_pri_tree_widget;
    QLabel* mp_mouse_pos_label;
    Primitive* mp_pri;
    ly::LayerWidget *mp_layer_widget;
    pm::ParamEditWidget *mp_param_edit_widget;
    pm::ParamRuleWidget *mp_param_rule_widget;
    PrimitiveCoordinateUi *mp_coordinate_table;
    PrimitiveAnchorUi *mp_anchor_table;
    QString m_current_path;

    // 用于收集在界面上选中的坐标点
    bool m_is_recording = false;
    bool m_is_shift_pressed = false;

    QVector<QPair<QPointF, sp::ShapePointGraphicsItem*>> m_chosen_coord_points;
    sp::ShapePointGraphicsItem* mp_polygen_cache_point = nullptr;
    QGraphicsPolygonItem m_preview_polygen;

    QAction* m_cache_action = nullptr;

    PriRulerDialog* mp_ruler_dialog = nullptr;

    bool m_has_connects = false;
};

}

#endif // PRIMITIVEWINDOW_H
