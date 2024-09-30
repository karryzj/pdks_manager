#ifndef PRISHAPEDIALOG_H
#define PRISHAPEDIALOG_H

#include <QDialog>
#include <QMap>
#include "attachTreeDefines.h"

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
class PriShapeDialog final : public QDialog
{
    Q_OBJECT
public:
    PriShapeDialog(const QString& shape_name,
                   bool add_type,
                   bool add_direction,
                   bool add_layer,
                   ly::LayerWidget* layer_widget,
                   Primitive* pri);
    ~PriShapeDialog() override;

public:
    void update_all(void);  // 后续可能会对ui进行更新，对应的更新操作都预留在此处。

    void init();

public:
    at::NodeType shape_node_type() const;

    at::NodeDirection shape_node_direction() const;

    QString shape_layer_info_name() const;

    QVector<pm::ParamDecl> shape_param_list() const;

    QVariant get_special_param(const QString& param_name) const;

    void init_ui_in_create_shape();

    void init_ui_in_edit_shape(sp::ShapeDrawGraphicsItem* shape_item);

    // 多边形特化
    bool is_extract_params() const;

    at::AttachTreeNode* editing_tree_node() const;

    // 设置回显
private:
    void setup_connect();

    void set_shape_node_type(at::NodeType);
    void set_shape_node_direction(at::NodeDirection);
    void set_shape_layer_info_name(const ly::LayerInfo* layerinfo);

    void set_shape_param_list(const QVector<QPair<QString, QVariant>>& param_list);
private:
    bool check_shape_is_extracted_params(const QVector<pm::ParamDecl> & param_list);


private slots:
    void update_combo_box_layer_info();

    void update_polygen_point_param_display(bool checked);

    void check_input();

private:
    QString m_shape_name;
    bool m_add_type ;
    bool m_add_direction;
    bool m_add_layer;
    ly::LayerWidget* mp_layer_widget;
    Primitive* mp_pri;

    bool m_is_extract_params = false;

    at::AttachTreeNode* mp_editing_tree_node = nullptr;

private:
    mutable QVector<pm::ParamDecl> m_params;
    QMap<QString, qreal> m_line_edit_name_to_value;
};

}

#endif // PRISHAPEDIALOG_H
