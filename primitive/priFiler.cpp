#include "priFiler.h"


#include <AttachTreeBaseNode.h>
#include <AttachTreeNode.h>
#include <Primitive.h>
#include <layInfo.h>
#include "attachTreeNodeMgr.h"
#include "attachTreeRootNode.h"
#include "layerMgr.h"
#include "paramMgr.h"
#include "shapeBase.h"
#include "ParamDecl.h"
#include "attachTreeUtils.h"
#include "pointE.h"


using namespace at;

namespace pr
{

PriFiler::PriFiler(Primitive *pri)
    : mp_pri(pri)
{

}

PriFiler::~PriFiler()
{

}

void PriFiler::save_primitive_to_file(QJsonObject &root_object)
{
    QJsonObject primitive_object;

    QJsonObject coord_anchor_obj;
    save_coord_anchor(coord_anchor_obj);
    primitive_object["coord_anchor"] = coord_anchor_obj;

    QJsonArray anchors_array;
    save_anchors(anchors_array);
    primitive_object["anchors"] = anchors_array;

    QJsonArray layer_array;
    save_layers(layer_array);
    primitive_object["layers"] = layer_array;

    QJsonObject param_rules_obj;
    save_param_rules(param_rules_obj);
    primitive_object["param_rules"] = param_rules_obj;

    QJsonObject node_obj;
    save_tree_nodes(node_obj);
    primitive_object["node"] = node_obj;

    root_object["primitive"] = primitive_object;
}

void PriFiler::load_primitive_from_file(const QJsonObject &root_object)
{
    QJsonObject primitive_object = root_object["primitive"].toObject();

    QJsonArray layers = primitive_object["layers"].toArray();
    PriFiler::load_layers(layers);

    QJsonObject param_manager = primitive_object["param_rules"].toObject();
    PriFiler::load_param_rules(param_manager);

    QJsonObject node_obj = primitive_object["node"].toObject();
    PriFiler::load_tree_nodes(node_obj);

    QJsonObject coord_anchor_obj = primitive_object["coord_anchor"].toObject();
    PriFiler::load_coord_anchor(coord_anchor_obj);

    QJsonArray anchors = primitive_object["anchors"].toArray();
    PriFiler::load_anchors(anchors);

    if(!m_has_match)
    {
        match_old_tree_node_to_new_tree_node_id();
        m_has_match = true;
    }

    mp_pri->tree_node_mgr()->update();

    init_pri_config();
}

void PriFiler::load_base_info_from_file(const QJsonObject &root_object)
{
    QJsonObject primitive_object = root_object["primitive"].toObject();

    QJsonArray layers = primitive_object["layers"].toArray();
    PriFiler::load_layers(layers);

    QJsonObject param_manager = primitive_object["param_rules"].toObject();
    PriFiler::load_param_rules(param_manager);

    QJsonArray anchors = primitive_object["anchors"].toArray();
    PriFiler::load_anchors_base(anchors);
}

void PriFiler::load_node_from_file(const QJsonObject &root_object)
{
    QJsonObject primitive_object = root_object["primitive"].toObject();

    QJsonObject node_obj = primitive_object["node"].toObject();
    PriFiler::load_tree_nodes(node_obj);

    QJsonObject coord_anchor_obj = primitive_object["coord_anchor"].toObject();
    PriFiler::load_coord_anchor(coord_anchor_obj);

    QJsonArray anchors = primitive_object["anchors"].toArray();
    PriFiler::load_anchors(anchors);

    if(!m_has_match)
    {
        match_old_tree_node_to_new_tree_node_id();
        m_has_match = true;
    }

    init_pri_config();

    mp_pri->tree_node_mgr()->update();
}

void PriFiler::load_primitive_from_file(const QJsonObject &root_object, sp::ShapePointGraphicsItem *point_item)
{
    QJsonObject primitive_object = root_object["primitive"].toObject();

    QJsonArray layers = primitive_object["layers"].toArray();
    PriFiler::load_layers(layers);

    QJsonObject param_manager = primitive_object["param_rules"].toObject();
    PriFiler::load_param_rules(param_manager);

    QJsonObject node_obj = primitive_object["node"].toObject();
    PriFiler::load_tree_nodes(node_obj, point_item);

    QJsonObject coord_anchor_obj = primitive_object["coord_anchor"].toObject();
    PriFiler::load_coord_anchor(coord_anchor_obj);

    QJsonArray anchors = primitive_object["anchors"].toArray();
    PriFiler::load_anchors(anchors);

    if(!m_has_match)
    {
        match_old_tree_node_to_new_tree_node_id();
        m_has_match = true;
    }

    mp_pri->tree_node_mgr()->update();

    emit mp_pri->signal_add_new_tree();
}

int PriFiler::save_coord_anchor(QJsonObject &coord_anchor_obj)
{
    at::AttachTreeUtils::AttachPointPosInf point_inf;
    at::AttachTreeUtils::get_coordinate_inf(mp_pri->at_root(), point_inf);

    coord_anchor_obj["x"] = point_inf.pos.x().to_str();
    coord_anchor_obj["y"] = point_inf.pos.y().to_str();
    coord_anchor_obj["node_id"] = point_inf.id;
    coord_anchor_obj["attach_point_idx"] = point_inf.attach_point_idx;
    return 0;
}

int PriFiler::load_coord_anchor(const QJsonObject &coord_anchor_obj)
{
    auto x = coord_anchor_obj["x"].toString();
    auto y = coord_anchor_obj["y"].toString();
    auto idx = coord_anchor_obj["attach_point_idx"].toDouble();

    at::AttachTreeUtils::AttachPointPosInf point_inf;
    AttachTreeNodeId node_id = (AttachTreeNodeId)coord_anchor_obj["node_id"].toDouble();
    at::AttachTreeNode* tree_node = get_new_tree_node_by_old_id(node_id);
    if(nullptr == tree_node)
    {
        return -1;
    }

    point_inf.id = tree_node->id();
    point_inf.attach_point_idx = idx;
    point_inf.pos = pm::PointE(x, y);

    at::AttachTreeUtils::set_coordinate_inf(mp_pri->at_root(), point_inf);
    return 0;
}

int PriFiler::load_coord_anchor_base(const QJsonObject &coord_anchor_obj)
{
    auto x = coord_anchor_obj["x"].toString();
    auto y = coord_anchor_obj["y"].toString();
    auto idx = coord_anchor_obj["attach_point_idx"].toDouble();

    at::AttachTreeUtils::AttachPointPosInf point_inf;

    // 此时treeNode还未建立，id设置为0
    point_inf.id = 0;
    point_inf.attach_point_idx = idx;
    point_inf.pos = pm::PointE(x, y);

    at::AttachTreeUtils::set_coordinate_inf(mp_pri->at_root(), point_inf);
    return 0;
}


int PriFiler::save_anchors(QJsonArray &anchors_array)
{
    QVector<at::AttachTreeUtils::AttachPointPosInf> anchors;
    at::AttachTreeUtils::get_anchors_inf(mp_pri->at_root(), anchors);
    mp_pri->set_json_anchors(anchors);
    for (int i = 0; i < anchors.size(); ++i)
    {
        QJsonObject anchor_obj;
        anchor_obj["x"] = anchors[i].pos.x().to_str();
        anchor_obj["y"] = anchors[i].pos.y().to_str();
        anchor_obj["node_id"] = anchors[i].id;
        anchor_obj["attach_point_idx"] = anchors[i].attach_point_idx;
        anchors_array.append(anchor_obj);
    }
    return 0;
}

int PriFiler::load_anchors(const QJsonArray &anchors_array)
{
    QVector<at::AttachTreeUtils::AttachPointPosInf> anchors;
    for (int i = 0; i < anchors_array.size(); ++i)
    {
        at::AttachTreeUtils::AttachPointPosInf anchor;
        QJsonObject anchor_obj = anchors_array[i].toObject();
        anchor.pos = pm::PointE(anchor_obj["x"].toString(), anchor_obj["y"].toString());
        AttachTreeNodeId node_id = (AttachTreeNodeId)anchor_obj["node_id"].toDouble();
        at::AttachTreeNode* tree_node = get_new_tree_node_by_old_id(node_id);
        if(nullptr == tree_node)
        {
            return -1;
        }
        anchor.id = tree_node->id();
        anchor.attach_point_idx = anchor_obj["attach_point_idx"].toInt();
        anchors.append(anchor);
    }
    at::AttachTreeUtils::set_anchors_inf(mp_pri->at_root(), anchors);
    return 0;
}

int PriFiler::load_anchors_base(const QJsonArray &anchors_array)
{
    QVector<at::AttachTreeUtils::AttachPointPosInf> anchors;
    for (int i = 0; i < anchors_array.size(); ++i)
    {
        at::AttachTreeUtils::AttachPointPosInf anchor;
        QJsonObject anchor_obj = anchors_array[i].toObject();
        anchor.pos = pm::PointE(anchor_obj["x"].toString(), anchor_obj["y"].toString());

        // 此时treeNode还未建立，id设置为0
        anchor.id = 0;
        anchor.attach_point_idx = anchor_obj["attach_point_idx"].toInt();
        anchors.append(anchor);
    }
    mp_pri->set_json_anchors(anchors);
    return 0;
}

int PriFiler::load_node_anchors(const QJsonArray &anchors_array)
{
    QVector<at::AttachTreeUtils::AttachPointPosInf> anchors;
    for (int i = 0; i < anchors_array.size(); ++i)
    {
        at::AttachTreeUtils::AttachPointPosInf anchor;
        QJsonObject anchor_obj = anchors_array[i].toObject();
        anchor.pos = pm::PointE(anchor_obj["x"].toString(), anchor_obj["y"].toString());
        AttachTreeNodeId node_id = (AttachTreeNodeId)anchor_obj["node_id"].toDouble();
        at::AttachTreeNode* tree_node = get_new_tree_node_by_old_id(node_id);
        if(nullptr == tree_node)
        {
            return -1;
        }

        anchor.id = tree_node->id();
        anchor.attach_point_idx = anchor_obj["attach_point_idx"].toInt();
        anchors.append(anchor);
    }

    at::AttachTreeUtils::set_anchors_inf(mp_pri->at_root(), anchors);
    return 0;
}

int PriFiler::save_layers( QJsonArray &layers)
{
    QList<ly::LayerInfo*> &layer_infos = mp_pri->layer_mgr()->get_layers();

    for(auto itor = layer_infos.begin(); itor != layer_infos.end(); itor++)
    {
        const ly::LayerInfo* layer_info = *itor;
        QJsonObject layer_object;
        layer_object["layer_name"] = layer_info->layer_name();

        QJsonObject style_object;
        style_object["border_line"] = layer_info->border_line();
        style_object["border_color"] = layer_info->border_color().name().mid(1);
        style_object["fill_line"] = layer_info->fill_line();
        style_object["fill_color"] = layer_info->fill_color().name().mid(1);
        layer_object["style"] = style_object;

        layers.append(layer_object);
    }
    return 0;
}

int PriFiler::load_layers(const QJsonArray &layers)
{
    for (int i = 0; i < layers.size(); ++i)
    {
        QJsonObject layer_object = layers[i].toObject();
        auto layer_name = layer_object["layer_name"].toString();
        // 这里需要额外进行查重判断。如果已经有同名的图层就不进行加载。
        if(mp_pri->layer_mgr()->get_layer_info_by_name(layer_name))
        {
            continue;
        }

        ly::LayerInfo *layer_info = new ly::LayerInfo();
        layer_info->set_layer_name(layer_name);
        // Access style object
        QJsonObject style_obj = layer_object["style"].toObject();
        layer_info->set_border_line(static_cast<ly::LayerInfo::Border_Style>(style_obj["border_line"].toInt()));
        layer_info->set_border_color(style_obj["border_color"].toString().toInt(nullptr, 16));
        layer_info->set_fill_line(static_cast<ly::LayerInfo::Fill_Style>(style_obj["fill_line"].toInt()));
        layer_info->set_fill_color(style_obj["fill_color"].toString().toInt(nullptr, 16));
        mp_pri->layer_mgr()->set_layers(layer_info);
    }
    return 0;
}

int PriFiler::save_param_rules( QJsonObject &param_rules)
{
    const auto &params = mp_pri->param_mgr()->params();
    QJsonArray param_array;
    for(auto itor = params.begin(); itor != params.end(); ++itor)
    {
        const auto param = *itor;
        QJsonObject param_object = param_to_json_obj(param);
        param_array.append(param_object);
    }
    param_rules["params"] = param_array;

    const auto &rules = mp_pri->param_mgr()->rules();
    QJsonArray rule_array;
    for(auto itor = rules.begin(); itor != rules.end(); ++itor)
    {
        rule_array.append(itor->to_str());
    }
    param_rules["rules"] = rule_array;
    return 0;
}

int PriFiler::load_param_rules(const QJsonObject &param_rules)
{
    QJsonArray param_array = param_rules["params"].toArray();
    for (const QJsonValue &json_obj : param_array)
    {
        pm::ParamDecl *param = json_obj_to_param(json_obj.toObject());
        mp_pri->param_mgr()->add_param(param);
    }

    QJsonArray rule_array = param_rules["rules"].toArray();
    for (const QJsonValue &json_obj : rule_array)
    {
        auto rule = pm::Rule(json_obj.toString());
        mp_pri->param_mgr()->add_rule(rule);
    }
    return 0;
}

int PriFiler::save_tree_nodes( QJsonObject &node_obj)
{
    // HINT@leixunyong。这里存在一个漏洞：在加载图元的时候，根节点信息没有被加载进来；在导入图元的时候，根节点信息不应该被导入进来。这里需要综合考虑这两件事。
    // QJsonObject root_node_obj;
    save_tree_root_node(node_obj);
    // node_obj["root_node"] = root_node_obj;

    QJsonArray child_node_array;
    save_tree_child_nodes(child_node_array);
    node_obj["children"] = child_node_array;

    return 0;
}

int PriFiler::load_tree_nodes(const QJsonObject &node_obj)
{
    // QJsonObject root_node_obj = node_obj["root_node"].toObject();
    // load_tree_root_node(root_node_obj);

    auto root_node = mp_pri->at_root();

    QJsonArray child_node_array =  node_obj["children"].toArray();
    // load_tree_children_nodes(child_node_array, root_node);
    load_root_children_nodes(child_node_array, root_node);

    return 0;
}

int PriFiler::load_tree_nodes(const QJsonObject &node_obj, sp::ShapePointGraphicsItem *point_item)
{
    QJsonArray child_node_array =  node_obj["children"].toArray();
    load_tree_child_nodes(child_node_array, point_item);
    return 0;
}

int PriFiler::save_tree_root_node( QJsonObject &root_node_obj)
{
    const auto &root = mp_pri->at_root();
    root_node_obj["id"] = 0;
    root_node_obj["parent_attach_point_idx"] = -1;
    QJsonArray attach_points_array;
    QJsonObject attach_point;
    attach_point["id"] = 0;
    attach_point["x"] = 0;
    attach_point["y"] = 0;
    attach_point["is_coord_anchor"] = root->is_coord_point(0);
    attach_point["is_anchor"] = true;
    attach_points_array.append(attach_point);
    root_node_obj["attach_points"] = attach_points_array;
    return 0;
}

int PriFiler::load_tree_root_node(const QJsonObject &root_node_obj)
{
    // HINT@leixunyong。还不知道需要加载什么。
    return 0;
}

int PriFiler::save_tree_child_nodes( QJsonArray &child_node_array)
{
    // 收集根节点下的子节点
    if(mp_pri->at_root()->children().size() < 1)
    {
        return 0;
    }

    const auto &root_children = mp_pri->at_root()->children();
    for (auto root_child_key : root_children.keys())
    {
        const auto &root_child_values = root_children.value(root_child_key);
        for (const auto &root_child_value : root_child_values)
        {
            // 处理当前节点
            QJsonObject tree_node_obj;
            int return_value = save_tree_child_node(root_child_value, tree_node_obj);
            Q_ASSERT(0 == return_value);
            child_node_array.append(tree_node_obj);
        }
    }

    /*current_tree_nodes = mp_pri->at_root()->children().begin().value();
    QVector<at::AttachTreeNode *> next_tree_nodes;

    while(current_tree_nodes.size() > 0)
    {
        for(auto itor = current_tree_nodes.begin(); itor != current_tree_nodes.end(); itor++)
        {
            at::AttachTreeNode * tree_node = *itor;
            // 处理当前节点
            QJsonObject tree_node_obj;
            int return_value = save_tree_child_node(tree_node, tree_node_obj);
            Q_ASSERT(0 == return_value);
            child_node_array.append(tree_node_obj);
            // 收集需要处理的子节点
            auto child_tree_node_map = tree_node->children();
            for(auto itor =  child_tree_node_map.begin(); itor != child_tree_node_map.end(); itor++)
            {
                const auto& child_tree_nodes = itor.value();
                for(auto tree_node : child_tree_nodes)
                {
                    next_tree_nodes.push_back(tree_node);
                }
            }
        }
        current_tree_nodes = next_tree_nodes;
        next_tree_nodes.resize(0);
    }*/

    return 0;
}

int PriFiler::load_tree_child_nodes(const QJsonArray &child_node_array)
{
    for(const auto& node_value : child_node_array)
    {
        QJsonObject child_node_obj = node_value.toObject();
        at::AttachTreeNode* new_child_tree_node = nullptr;

        int return_value = load_tree_child_node(new_child_tree_node, child_node_obj);
        Q_ASSERT(0 == return_value);
    }
    return 0;
}

int PriFiler::load_tree_children_nodes(const QJsonArray &child_node_array, at::AttachTreeNode *child_tree_node)
{
    for(const auto& node_value : child_node_array)
    {
        QJsonObject child_node_obj = node_value.toObject();

        int return_value = load_tree_child_node(child_tree_node, child_node_obj);
        Q_ASSERT(0 == return_value);
    }
    return 0;
}

int PriFiler::load_root_children_nodes(const QJsonArray &child_node_array, at::AttachTreeRootNode *root_node)
{
    for(const auto& node_value : child_node_array)
    {
        QJsonObject child_node_obj = node_value.toObject();

        int return_value = load_root_child_node(root_node, child_node_obj);
        Q_ASSERT(0 == return_value);
    }
    return 0;
}

int PriFiler::load_tree_child_nodes(const QJsonArray &child_node_array, sp::ShapePointGraphicsItem *point_item)
{
    auto current_root_node = at::AttachTreeUtils::attach_tree_node_point_item_in(point_item, mp_pri->at_root());
    int parent_node_idx = at::AttachTreeUtils::point_item_index_in_tree_node(current_root_node, point_item);
    at::AttachTreeUtils::point_item_index_in_tree_node(current_root_node, point_item);
    for(const auto& node_value : child_node_array)
    {
        QJsonObject child_node_obj = node_value.toObject();
        int return_value = load_tree_child_node(current_root_node, child_node_obj, true, parent_node_idx);
        Q_ASSERT(0 == return_value);
    }
    return 0;
}

int PriFiler::save_tree_child_node(at::AttachTreeNode * tree_node, QJsonObject &child_node_obj)
{
    at::AttachTreeNodeId parent_node_id;
    int parent_node_index =  tree_node->get_parent_attach_point_idx();;
    if(tree_node->parent_node() == mp_pri->at_root())
    {
        parent_node_id = INVALID_ATTACH_TREE_NODE_ID;
    }
    else
    {
        auto parent_tree_node = dynamic_cast<at::AttachTreeNode *>(tree_node->parent_node());
        parent_node_id = parent_tree_node->id();
    }

    int return_value = save_tree_child_node_parent_node_info(parent_node_id, parent_node_index, child_node_obj);
    Q_ASSERT(0 == return_value);

    return_value = save_tree_child_node_id_info(tree_node->id(), child_node_obj);
    Q_ASSERT(0 == return_value);

    return_value = save_tree_child_node_attach_points_info(tree_node, child_node_obj);
    Q_ASSERT(0 == return_value);

    return_value = save_tree_child_node_shape_name_info(tree_node->shape_name(), child_node_obj);
    Q_ASSERT(0 == return_value);

    return_value = save_tree_child_node_layer_info(tree_node->layer_info(), child_node_obj);
    Q_ASSERT(0 == return_value);

    return_value = save_tree_child_node_type_and_direction_info(tree_node->node_type(), tree_node->node_direction(), child_node_obj);
    Q_ASSERT(0 == return_value);

    return_value = save_tree_child_node_params(tree_node->shape()->params(), child_node_obj);
    Q_ASSERT(0 == return_value);

    QMap<int, QVector<at::AttachTreeNode *>> sub_children_map = tree_node->children();
    if (sub_children_map.size() > 0)
    {
        QJsonArray children_obj;
        for (auto sit = sub_children_map.begin(); sit != sub_children_map.end(); ++sit)
        {
            // int key = sit.key();
            const auto &values = sit.value();
            for (const auto &sub_child : values)
            {
                QJsonObject sub_children_obj;
                save_tree_child_node(sub_child, sub_children_obj);
                children_obj.append(sub_children_obj);
            }
        }
        child_node_obj["children"] = children_obj;
    }

    return 0;
}

int PriFiler::load_root_child_node(at::AttachTreeRootNode * root_node, const QJsonObject &child_node_obj)
{
    at::AttachTreeNodeId parent_node_old_id = INVALID_ATTACH_TREE_NODE_ID;
    int parent_attach_point_idx = -1;
    int return_value = load_tree_child_node_parent_node_info(parent_node_old_id, parent_attach_point_idx, child_node_obj);
    Q_ASSERT(0 == return_value);

    at::AttachTreeNodeId id = INVALID_ATTACH_TREE_NODE_ID;
    return_value = load_tree_child_node_id_info(id, child_node_obj);
    Q_ASSERT(0 == return_value);

    QString shape_name;
    return_value = load_tree_child_node_shape_name_info(shape_name, child_node_obj);
    Q_ASSERT(0 == return_value);

    ly::LayerInfo* layer_info = nullptr;
    return_value = load_tree_child_node_layer_info(layer_info, child_node_obj);
    Q_ASSERT(0 == return_value);

    at::NodeType node_type = at::NodeType::NONE;
    at::NodeDirection node_direction = at::NodeDirection::NONE;
    return_value = load_tree_child_node_type_and_direction_info(node_type, node_direction, child_node_obj);
    Q_ASSERT(0 == return_value);

    QVector<pm::ParamDecl> params;
    return_value = load_tree_child_node_params(params, child_node_obj);
    Q_ASSERT(0 == return_value);

    at::AttachTreeNode * tree_node;
    root_node->set_update_children(false);
    tree_node =  root_node->add_child(parent_attach_point_idx, shape_name, params, node_type, node_direction, layer_info);
    root_node->set_update_children(true);
    m_old_tree_node_id_to_new_tree_node[id] = tree_node;
    QJsonArray sub_children = child_node_obj["children"].toArray();
    if (sub_children.size() > 0)
    {
        for (const auto &sub_child : sub_children)
        {
            load_tree_child_node(tree_node, sub_child.toObject());
        }
    }

    // QMap<int, QVector<at::AttachTreeNode *> >  children_map;
    // return_value = load_tree_child_node_children(children_map, child_node_obj);
    // Q_ASSERT(0 == return_value);

    return 0;
}

int PriFiler::load_tree_child_node(at::AttachTreeBaseNode * root_node, const QJsonObject &child_node_obj, bool user_defined_root_node,  int parent_attach_point_idx)
{
    int return_value = -1;

    at::AttachTreeNodeId parent_node_old_id = INVALID_ATTACH_TREE_NODE_ID;
    if(!user_defined_root_node)
    {
        return_value  = load_tree_child_node_parent_node_info(parent_node_old_id, parent_attach_point_idx, child_node_obj);
        Q_ASSERT(0 == return_value);
    }

    at::AttachTreeNodeId id = INVALID_ATTACH_TREE_NODE_ID;
    return_value = load_tree_child_node_id_info(id, child_node_obj);
    Q_ASSERT(0 == return_value);

    QString shape_name;
    return_value = load_tree_child_node_shape_name_info(shape_name, child_node_obj);
    Q_ASSERT(0 == return_value);

    ly::LayerInfo* layer_info = nullptr;
    return_value = load_tree_child_node_layer_info(layer_info, child_node_obj);
    Q_ASSERT(0 == return_value);

    at::NodeType node_type = at::NodeType::NONE;
    at::NodeDirection node_direction = at::NodeDirection::NONE;
    return_value = load_tree_child_node_type_and_direction_info(node_type, node_direction, child_node_obj);
    Q_ASSERT(0 == return_value);

    QVector<pm::ParamDecl> params;
    return_value = load_tree_child_node_params(params, child_node_obj);
    Q_ASSERT(0 == return_value);

    at::AttachTreeNode * tree_node;
    tree_node =  root_node->add_child(parent_attach_point_idx, shape_name, params, node_type, node_direction, layer_info);

    QVector<QPointF> attach_points;
    return_value = load_tree_child_node_attach_points_info(tree_node, child_node_obj);
    Q_ASSERT(0 == return_value);

    QJsonArray sub_children = child_node_obj["children"].toArray();
    if (sub_children.size() > 0)
    {
        for (const auto &sub_child : sub_children)
        {
            load_tree_child_node(tree_node, sub_child.toObject());
        }
    }

    m_old_tree_node_id_to_new_tree_node[id] = tree_node;
    return 0;
}

int PriFiler::load_tree_child_node(at::AttachTreeNode * root_node, const QJsonObject &child_node_obj)
{
    at::AttachTreeNodeId parent_node_old_id = INVALID_ATTACH_TREE_NODE_ID;
    int parent_attach_point_idx = -1;
    int return_value = load_tree_child_node_parent_node_info(parent_node_old_id, parent_attach_point_idx, child_node_obj);
    Q_ASSERT(0 == return_value);

    at::AttachTreeNodeId id = INVALID_ATTACH_TREE_NODE_ID;
    return_value = load_tree_child_node_id_info(id, child_node_obj);
    Q_ASSERT(0 == return_value);

    QString shape_name;
    return_value = load_tree_child_node_shape_name_info(shape_name, child_node_obj);
    Q_ASSERT(0 == return_value);

    ly::LayerInfo* layer_info = nullptr;
    return_value = load_tree_child_node_layer_info(layer_info, child_node_obj);
    Q_ASSERT(0 == return_value);

    at::NodeType node_type = at::NodeType::NONE;
    at::NodeDirection node_direction = at::NodeDirection::NONE;
    return_value = load_tree_child_node_type_and_direction_info(node_type, node_direction, child_node_obj);
    Q_ASSERT(0 == return_value);

    QVector<pm::ParamDecl> params;
    return_value = load_tree_child_node_params(params, child_node_obj);
    Q_ASSERT(0 == return_value);

    at::AttachTreeNode * tree_node;
    root_node->set_update_children(false);
    tree_node =  root_node->add_child(parent_attach_point_idx, shape_name, params, node_type, node_direction, layer_info);
    root_node->set_update_children(true);
    // HINT@leixunyong。在节点生成后才能生成附着点。
    return_value = load_tree_child_node_attach_points_info(tree_node, child_node_obj);
    Q_ASSERT(0 == return_value);

    m_old_tree_node_id_to_new_tree_node[id] = tree_node;
    QJsonArray sub_children = child_node_obj["children"].toArray();
    if (sub_children.size() > 0)
    {
        for (const auto &sub_child : sub_children)
        {
            load_tree_child_node(tree_node, sub_child.toObject());
        }
    }

    return 0;
}

int PriFiler::save_tree_child_node_parent_node_info(const at::AttachTreeNodeId& parent_node_id, int parent_attach_point_idx, QJsonObject &child_node)
{
    child_node["parent_node_id"] = parent_node_id;
    child_node["parent_attach_point_idx"] = parent_attach_point_idx;
    return 0;
}

int PriFiler::load_tree_child_node_parent_node_info(at::AttachTreeNodeId& parent_node_id, int& parent_attach_point_idx, const QJsonObject &child_node)
{
    parent_node_id = child_node["parent_node_id"].toInt();
    parent_attach_point_idx = child_node["parent_attach_point_idx"].toInt();
    return 0;
}

int PriFiler::save_tree_child_node_id_info(const at::AttachTreeNodeId& id, QJsonObject &child_node)
{
    child_node["id"] = id;
    return 0;
}

int PriFiler::load_tree_child_node_id_info(at::AttachTreeNodeId& id, const QJsonObject &child_node)
{
    id = child_node["id"].toInt();
    return 0;
}

int PriFiler::save_tree_child_node_attach_points_info(at::AttachTreeNode *node, QJsonObject &child_node)
{
    QJsonArray attach_points_array;

    for(int idx = 0; idx < node->shape()->attach_points().size(); idx++)
    {
        auto point = at::AttachTreeUtils::attach_exp_point(node, idx, false);  // HINT@leixunyong。这里的附着点的表达式需要保存吗？
        int id = idx;
        bool is_anchor = node->is_anchor_point(idx);
        bool is_coord_anchor = node->is_coord_point(idx);
        auto x = point.x();
        auto y = point.y();

        QJsonObject attach_point_obj;
        attach_point_obj["id"] = id;
        attach_point_obj["is_anchor"] = is_anchor;
        attach_point_obj["is_coord_anchor"] = is_coord_anchor;
        attach_point_obj["x"] = x.to_str();
        attach_point_obj["y"] = y.to_str();

        attach_points_array.append(attach_point_obj);
    }

    child_node["attach_points"] = attach_points_array;
    return 0;
}

int PriFiler::load_tree_child_node_attach_points_info(at::AttachTreeNode *node, const QJsonObject &child_node)
{
    QJsonArray attach_points_array = child_node["attach_points"].toArray();
    const auto& point_items = node->shape()->point_graphics_items();

    Q_ASSERT(attach_points_array.size() == point_items.size());

    for(int idx = 0; attach_points_array.size(); idx++)
    {
        QJsonObject attach_point_obj = attach_points_array.at(idx).toObject();

        int id = attach_point_obj["id"].toInt();
        bool is_anchor = attach_point_obj["is_anchor"].toBool();
        bool is_coord_anchor = attach_point_obj["is_coord_anchor"].toBool();
        QString x = attach_point_obj["x"].toString();
        QString y = attach_point_obj["y"].toString();   // HINT@leixunyong。这里的附着点的表达式没有找到加载的方式。

        point_items[id]->set_anchor_point(is_anchor);
        point_items[id]->set_coor_anchor_point(is_coord_anchor);
        return 0;
    }
}
int PriFiler::save_tree_child_node_shape_name_info(const QString& shape_name, QJsonObject &child_node)
{
    child_node["shape_name"] = shape_name;
    return 0;
}

int PriFiler::load_tree_child_node_shape_name_info(QString& shape_name, const QJsonObject &child_node)
{
    shape_name = child_node["shape_name"].toString();
    return 0;
}

int PriFiler::save_tree_child_node_layer_info(const ly::LayerInfo* layer, QJsonObject &child_node)
{
    QString layer_name = layer->layer_name();
    child_node["layer"] = layer_name;
    return 0;
}

int PriFiler::load_tree_child_node_layer_info(ly::LayerInfo*& layer_info, const QJsonObject &child_node)
{
    QString layer_name = child_node["layer"].toString();
    layer_info = mp_pri->layer_mgr()->get_layer_info_by_name(layer_name);
    return 0;
}

int PriFiler::save_tree_child_node_type_and_direction_info(at::NodeType node_type, at::NodeDirection node_direction, QJsonObject &child_node)
{
    child_node["node_type"] = static_cast<int>(node_type);
    child_node["node_direction"] = static_cast<int>(node_direction);
    return 0;
}

int PriFiler::load_tree_child_node_type_and_direction_info(at::NodeType& node_type, at::NodeDirection& node_direction, const QJsonObject &child_node)
{
    node_type  = static_cast<at::NodeType>(child_node["node_type"].toInt());
    node_direction = static_cast<at::NodeDirection>(child_node["node_direction"].toInt());
    return 0;
}

int PriFiler::save_tree_child_node_params(const QVector<pm::ParamDecl>& params, QJsonObject &child_node)
{
    QJsonArray params_json_array;
    for(int idx = 0; idx < params.size(); idx++)
    {
        const auto& param = params[idx];
        QJsonObject param_json_obj = param_to_json_obj(&param);
        params_json_array.append(param_json_obj);
    }
    child_node["params"] = params_json_array;

    return 0;
}

int PriFiler::load_tree_child_node_params(QVector<pm::ParamDecl>& params, const QJsonObject &child_node)
{
    QJsonArray params_json_array = child_node["params"].toArray();
    for(int idx = 0; idx < params_json_array.size(); idx++)
    {
        QJsonObject param_object = params_json_array[idx].toObject();
        pm::ParamDecl* param = json_obj_to_param(param_object);
        params.append(*param);
    }
    return 0;
}

QJsonObject PriFiler::q_color_to_json_obj(const QColor &color)
{
    QJsonObject color_json;

    int red = color.red();
    int green = color.green();
    int blue = color.blue();
    int alpha = color.alpha();

    color_json["red"] = red;
    color_json["green"] = green;
    color_json["blue"] = blue;
    color_json["alpha"] = alpha;

    return color_json;
}

QColor PriFiler::json_obj_to_q_color(const QJsonObject& color_json)
{
    int red =  color_json["red"].toInt();
    int green = color_json["green"].toInt();
    int blue = color_json["blue"].toInt();
    int alpha = color_json["alpha"].toInt();
    return QColor(red, green, blue, alpha);
}

QJsonObject PriFiler::param_to_json_obj(const pm::ParamDecl *param)
{
    QJsonObject param_json_obj;
    const QString& desc = param->desc();
    param_json_obj["desc"] = desc;

    const QString& param_key = param->key();
    param_json_obj["key"] = param_key;
    param_json_obj["value"] = param->expression();

    return param_json_obj;
}

pm::ParamDecl *PriFiler::json_obj_to_param(const QJsonObject &param_json_obj)
{
    QString desc =  param_json_obj["desc"].toString();
    QString param_key =  param_json_obj["key"].toString();

    pm::ParamDecl* param = nullptr;

    QString param_value = param_json_obj["value"].toString();
    param = new pm::ParamDecl(param_key, param_value, desc);

    return param;
}

at::AttachTreeNode* PriFiler::get_new_tree_node_by_old_id(at::AttachTreeNodeId old_id) const
{
    auto itor = m_old_tree_node_id_to_new_tree_node.find(old_id);
    if(itor != m_old_tree_node_id_to_new_tree_node.end())
    {
        return itor.value();
    }
    return nullptr;
}

void PriFiler::match_old_tree_node_to_new_tree_node_id()
{
    for(auto itor = m_old_tree_node_id_to_new_tree_node.begin(); itor != m_old_tree_node_id_to_new_tree_node.end(); itor++)
    {
        auto tree_node = itor.value();
        tree_node->set_new_id();
    }
}

void PriFiler::init_pri_config() const
{
    sp::ShapePointGraphicsItem::cachedItem = mp_pri->at_root()->origin_point();
}

}
