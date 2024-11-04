#ifndef PRIFILER_H
#define PRIFILER_H

#include <QPoint>
#include <QVector>
#include <QColor>
#include <QMap>

/* 图元保存加载说明。
 * 保存加载是通过JSON文件来实现的。保存加载的内容有：
 * 1. anchors。锚点信息。
 * 2. coord_anchor。坐标锚点信息。
 * 3. layers。图层信息。
 * 4. nodes。附着树节点信息。
 * 5. param_rules。参数信息。
 * 在加载的时候，分为2种情况：
 * 1. 加载一个磁盘上的Json文件，转为一个新的图元。此时需要加载全部1-5的信息；
 * 2. 加载一个磁盘上的Json文件，作为新的节点加入到已有的图元中。此时仅需加载3-5；
 */
#include "attachTreeDefines.h"
#include "qjsonobject.h"
#include <QJsonArray.h>
#include "priCommon.h"
namespace  at
{
class AttachTreeNode;
class AttachTreeBaseNode;
class AttachTreeRootNode;
}

namespace ly
{
class LayerInfo;
}

namespace pm
{
class ParamDecl;
}

namespace sp
{
class ShapePointGraphicsItem;
}

namespace pr
{
// 此类专门用于图元的保存和加载
// 如果由此类导致保存加载问题请联系@leixunyong。
class Primitive;
class PRI_PUBLIC PriFiler final
{
public:
    PriFiler(Primitive* pri);
    ~PriFiler();

    void save_primitive_to_file(QJsonObject& root_object);
    // 将对象加载到一个全新的图元中。
    void load_primitive_from_file(const QJsonObject& root_object);
    void load_base_info_from_file(const QJsonObject &root_object);
    void load_node_from_file(const QJsonObject &root_object);
    // 将对象加载到一个已有的图元的某个附着点下。
    void load_primitive_from_file(const QJsonObject& root_object, sp::ShapePointGraphicsItem* point_item);

    void load_py_info_from_file(const QJsonObject &root_object);
private:
    int save_coord_anchor(QJsonObject &coord_anchor_obj);
    int load_coord_anchor(const QJsonObject &coord_anchor_obj);

    // 获取coord的基础信息，不包含id
    int load_coord_anchor_base(const QJsonObject &coord_anchor_obj);
    int save_anchors( QJsonArray &anchors);
    int load_anchors(const QJsonArray &anchors);
    int load_node_anchors(const QJsonArray &anchors_array);

    // 获取anchor的基础信息，不包含id
    int load_anchors_base(const QJsonArray &anchors_array);

    int save_layers(QJsonArray &layers);
    int load_layers(const QJsonArray &layers);

    int save_param_rules(QJsonObject &param_rules);
    int load_param_rules(const QJsonObject &param_rules);

    int save_tree_nodes( QJsonObject &node_obj);
    int load_tree_nodes(const QJsonObject &node_obj);
    int load_tree_nodes(const QJsonObject &node_obj, sp::ShapePointGraphicsItem* point_item);

    int load_py_layers(const QJsonArray &layers);
    int load_py_anchors(const QJsonArray &anchors_array);

    void save_arc_length(QJsonObject &arc_len);
    void load_arc_length(const QJsonObject &arc_len);

private:
    int save_tree_root_node( QJsonObject &root_node_obj);
    int load_tree_root_node(const QJsonObject &root_node_obj);

    int save_tree_child_nodes( QJsonArray &child_node_array);
    int load_tree_child_nodes(const QJsonArray &child_node_array);
    int load_tree_child_nodes(const QJsonArray &child_node_array, sp::ShapePointGraphicsItem* point_item);
    int load_tree_children_nodes(const QJsonArray &child_node_array, at::AttachTreeNode *child_tree_node);
    int load_root_children_nodes(const QJsonArray &child_node_array, at::AttachTreeRootNode *child_tree_node);

private:
    /* tree node分为2种。
     * 1. root node。根节点暂时不知道要记什么。
     * 2. child node。子节点需要记以下信息。
     * 父信息：
     * parent_node_id,
     * parent_attach_point_idx
     * 当前节点信息：
     * id,
     * attach_points,
     * shape_name,
     * layer,
     * node_type,
     * node_direction,
     * params,
     * 子信息：
     * children,
     */
    int save_tree_child_node( at::AttachTreeNode * tree_node, QJsonObject &child_node_obj);
    int load_tree_child_node(at::AttachTreeNode * root_node, const QJsonObject &child_node_obj);
    int load_root_child_node(at::AttachTreeRootNode * root_node, const QJsonObject &child_node_obj);
    int load_tree_child_node(at::AttachTreeBaseNode * tree_node, const QJsonObject &child_node_obj, bool user_defined_root_node, int parent_node_idx = -1);

    int save_tree_child_node_parent_node_info(const at::AttachTreeNodeId& parent_node_id, int parent_attach_point_idx, QJsonObject &child_node);
    int load_tree_child_node_parent_node_info(at::AttachTreeNodeId& parent_node_id, int& parent_attach_point_idx, const QJsonObject &child_node);

    int save_tree_child_node_id_info(const at::AttachTreeNodeId& id, QJsonObject &child_node);
    int load_tree_child_node_id_info(at::AttachTreeNodeId& id, const QJsonObject &child_node);

    int save_tree_child_node_attach_points_info(at::AttachTreeNode *node, QJsonObject &child_node);
    int load_tree_child_node_attach_points_info(at::AttachTreeNode *node, const QJsonObject &child_node);

    int save_tree_child_node_shape_name_info(const QString& shape_name, QJsonObject &child_node);
    int load_tree_child_node_shape_name_info(QString& shape_name, const QJsonObject &child_node);

    int save_tree_child_node_layer_info(const ly::LayerInfo* layer_info, QJsonObject &child_node);
    int load_tree_child_node_layer_info(ly::LayerInfo*& layer_info, const QJsonObject &child_node);

    int save_tree_child_node_type_and_direction_and_boolean_subtract_type_info(at::NodeType node_type, at::NodeDirection node_direction,  at::NodeBooleanSubtractType node_boolean_subtract_type, QJsonObject &child_node);
    int load_tree_child_node_type_and_direction_and_boolean_subtract_type_info(at::NodeType& node_type, at::NodeDirection& node_direction, at::NodeBooleanSubtractType& node_boolean_subtract_type,  const QJsonObject &child_node);

    int save_tree_child_node_params(const QVector<pm::ParamDecl>& params, QJsonObject &child_node);
public://下面这个函数暴漏给c++ pcell调用
    int load_tree_child_node_params(QVector<pm::ParamDecl>& params, const QJsonObject &child_node);

private:
    QJsonObject q_color_to_json_obj(const QColor& color);
    QColor json_obj_to_q_color(const QJsonObject& color_obj);

    QJsonObject param_to_json_obj(const pm::ParamDecl* param);
    pm::ParamDecl* json_obj_to_param(const QJsonObject& param_object);

    // 通过json传入的id获取新的node节点
    at::AttachTreeNode* get_new_tree_node_by_old_id(at::AttachTreeNodeId old_id) const;

    void match_old_tree_node_to_new_tree_node_id();

    void init_pri_config() const;

private:
    Primitive* mp_pri;
    QMap<at::AttachTreeNodeId, at::AttachTreeNode*> m_old_tree_node_id_to_new_tree_node;

    bool m_has_match = false;
};
}



#endif // PRIFILER_H
