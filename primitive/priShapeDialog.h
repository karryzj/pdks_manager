#ifndef PRISHAPEDIALOG_H
#define PRISHAPEDIALOG_H

#include <QDialog>
#include <QMap>
#include "attachTreeDefines.h"


class QVBoxLayout;
namespace at
{
class AttachTreeNode;
}

namespace pm
{
class ParamMgr;
class ParamDecl;
}

namespace ly
{
class LayerInfo;
class LayerWidget;
}

namespace sp
{
class ShapeDrawGraphicsItem;
}

namespace pr
{
class PriLayerInfoComboBox;
class Primitive;
class PriCurveInputHBoxLayout;
class PriShapeDialog final : public QDialog
{
    Q_OBJECT
public:
    PriShapeDialog(const QString& shape_name,
                   bool add_type,
                   bool add_direction,
                   bool add_boolean_subtract_type,
                   bool add_layer,
                   ly::LayerWidget* layer_widget,
                   Primitive* pri);
    ~PriShapeDialog() override;

public:
    void update_all(void);  // 后续可能会对ui进行更新，对应的更新操作都预留在此处。

    void init();

    Primitive* pri() const;

public:
    at::NodeType shape_node_type() const;

    at::NodeDirection shape_node_direction() const;

    at::NodeBooleanSubtractType shape_node_boolean_subtract_type() const;

    QString shape_layer_info_name() const;

    const QVector<pm::ParamDecl>& shape_param_list() const;

    QVariant get_special_param(const QString& param_name) const;

    void init_ui_in_create_shape();

    void init_ui_in_edit_shape(sp::ShapeDrawGraphicsItem* shape_item);

    at::AttachTreeNode* editing_tree_node() const;

private:
    void setup_connect();

    void set_shape_node_type(at::NodeType);
    void set_shape_node_direction(at::NodeDirection);
    void set_shape_node_boolean_subtract_type(at::NodeBooleanSubtractType);
    void set_shape_layer_info_name(const ly::LayerInfo* layerinfo);
    void set_shape_param_list(const QVector<QPair<QString, QVariant>>& param_list);

    void update_line_edits_by_params();

    bool equation_is_vaild(const QString& equation) const;
    bool fixed_point_set_is_valid(const QString& equation) const;

    void process_ui_for_curve_edit(const QVector<QPair<QString, QVariant>>& param_pairs);
signals:
    void add_new_param_to_param_mgr(const QString& name, const QString& expression, const QString& desc);
    bool check_shape_is_extracted_params(const QVector<pm::ParamDecl> & param_list);

private slots:
    void update_combo_box_layer_info();

    void update_combo_box_boolean_subtract_type_info();

    void update_params();

    void accept_modifies();

    void add_equation_input_widget();

    void add_fixed_point_set_input_widget();

    void remove_curve_input_layout(PriCurveInputHBoxLayout* layout);
    void update_polygen_point_param_display(bool checked, const QString& param_name);
private:
    QString m_shape_name;
    bool m_add_type ;
    bool m_add_direction;
    bool m_add_boolean_subtract_type;
    bool m_add_layer;
    ly::LayerWidget* mp_layer_widget;
    Primitive* mp_pri;

    at::AttachTreeNode* mp_editing_tree_node = nullptr;

    QVBoxLayout* m_context_layout;

    QVector<QPair<PriCurveInputHBoxLayout*, bool>> m_layouts;

private:
    QVector<pm::ParamDecl> m_params;
    QMap<QString, qreal> m_line_edit_name_to_value;

    int m_add_equation_count = 0;
    int m_add_fixed_point_count = 0;

};

}

#endif // PRISHAPEDIALOG_H
