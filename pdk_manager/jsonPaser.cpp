#include "pointE.h"
#include "layInfo.h"
#include "paramDecl.h"
#include "rule.h"
#include "jsonPaser.h"
#include "pdkInstance.h"
#include "layerMgr.h"
#include "paramMgr.h"

namespace pdk
{
JsonPaser::JsonPaser(PdkInstance* pdk_instance) : mp_pdk_instance(pdk_instance)
{

}

JsonPaser::~JsonPaser()
{

}

void JsonPaser::save_coord_anchor(PointInf point_inf, QJsonObject &coord_anchor_obj)
{
    coord_anchor_obj["x"] = point_inf.pos.x().to_str();
    coord_anchor_obj["y"] = point_inf.pos.y().to_str();
    coord_anchor_obj["node_id"] = point_inf.id;
    coord_anchor_obj["attach_point_idx"] = point_inf.attach_point_idx;
}

void JsonPaser::load_coord_anchor(const QJsonObject &coord_anchor_obj, PointInf &point_inf)
{
    auto x = coord_anchor_obj["x"].toString();
    auto y = coord_anchor_obj["y"].toString();
    auto idx = coord_anchor_obj["attach_point_idx"].toDouble();
    auto node_id = coord_anchor_obj["node_id"].toDouble();

    point_inf.id = node_id;
    point_inf.attach_point_idx = idx;
    point_inf.pos = pm::PointE(x, y);
}

void JsonPaser::save_anchors(QVector<PointInf> anchors, QJsonArray &anchors_array)
{
    for (int i = 0; i < anchors.size(); ++i)
    {
        QJsonObject anchor_obj;
        anchor_obj["x"] = anchors[i].pos.x().to_str();
        anchor_obj["y"] = anchors[i].pos.y().to_str();
        anchor_obj["node_id"] = anchors[i].id;
        anchor_obj["attach_point_idx"] = anchors[i].attach_point_idx;
        anchors_array.append(anchor_obj);
    }
}

void JsonPaser::load_anchors(const QJsonArray &anchors_array, QVector<PointInf> &anchors)
{
    for (int i = 0; i < anchors_array.size(); ++i)
    {
        PointInf anchor;
        QJsonObject anchor_obj = anchors_array[i].toObject();
        anchor.pos = pm::PointE(anchor_obj["x"].toString(), anchor_obj["y"].toString());
        anchor.id = anchor_obj["node_id"].toInt();
        anchor.attach_point_idx = anchor_obj["attach_point_idx"].toInt();
        anchors.append(anchor);
    }
}

bool JsonPaser::contains_layer(const QList<ly::LayerInfo*> &layer_infos, const QString &layer_name)
{
    auto it = std::find_if(layer_infos.begin(), layer_infos.end(), [&layer_name](const ly::LayerInfo* layer_info)
    {
        return layer_info && layer_info->layer_name() == layer_name;
    });
    return it != layer_infos.end();
}

void JsonPaser::save_layers(const QList<ly::LayerInfo*> &layer_infos, QJsonArray &layers)
{
    for(const auto &layer_info: layer_infos)
    {
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
}

void JsonPaser::load_layers(const QJsonArray &layers, QList<ly::LayerInfo*> &layer_infos)
{
    for (int i = 0; i < layers.size(); ++i)
    {
        QJsonObject layer_object = layers[i].toObject();
        auto layer_name = layer_object["layer_name"].toString();
        // 这里需要额外进行查重判断。如果已经有同名的图层就不进行加载。
        if (contains_layer(layer_infos, layer_name))
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
        layer_infos.append(layer_info);
    }
}

QJsonObject JsonPaser::param_to_json_obj(const pm::ParamDecl *param)
{
    QJsonObject param_json_obj;
    const QString& desc = param->desc();
    param_json_obj["desc"] = desc;

    const QString& param_key = param->key();
    param_json_obj["key"] = param_key;
    param_json_obj["value"] = param->expression();

    return param_json_obj;
}

pm::ParamDecl *JsonPaser::json_obj_to_param(const QJsonObject &param_json_obj)
{
    QString desc =  param_json_obj["desc"].toString();
    QString param_key =  param_json_obj["key"].toString();

    pm::ParamDecl* param = nullptr;

    QString param_value = param_json_obj["value"].toString();
    param = new pm::ParamDecl(param_key, param_value, desc);

    return param;
}

void JsonPaser::save_params(const QList<pm::ParamDecl*> &params, QJsonObject &param_rules)
{
    QJsonArray param_array;
    for(const auto param : params)
    {
        QJsonObject param_object = param_to_json_obj(param);
        param_array.append(param_object);
    }
    param_rules["params"] = param_array;
}

void JsonPaser::load_params(const QJsonObject &param_rules, QList<pm::ParamDecl*> &params)
{
    QJsonArray param_array = param_rules["params"].toArray();
    for (const QJsonValue &json_obj : param_array)
    {
        pm::ParamDecl *param = json_obj_to_param(json_obj.toObject());
        params.append(param);
    }
}

void JsonPaser::save_rules(const QVector<pm::Rule> &rules, QJsonObject &param_rules)
{
    QJsonArray rule_array;
    for (const auto &rule : rules)
    {
        rule_array.append(rule.to_str());
    }
    param_rules["rules"] = rule_array;
}

void JsonPaser::load_rules(const QJsonObject &param_rules, QVector<pm::Rule> &rules)
{
    QJsonArray rule_array = param_rules["rules"].toArray();
    for (const QJsonValue &json_obj : rule_array)
    {
        auto rule = pm::Rule(json_obj.toString());
        rules.append(rule);
    }
}

void JsonPaser::save_pcell_to_file(const QJsonObject &root_object)
{
    QJsonObject primitive_object;

    QJsonObject coord_anchor_obj;
    auto coord_anchor = mp_pdk_instance->coord_anchor();
    save_coord_anchor(coord_anchor,coord_anchor_obj);
    primitive_object["coord_anchor"] = coord_anchor_obj;

    QJsonArray anchors_array;
    auto anchors = mp_pdk_instance->anchors();
    save_anchors(anchors, anchors_array);
    primitive_object["anchors"] = anchors_array;

    QJsonArray layer_array;
    auto layers = mp_pdk_instance->layer_mgr()->get_layers();
    save_layers(layers, layer_array);
    primitive_object["layers"] = layer_array;

    QJsonObject params_obj;
    auto params = mp_pdk_instance->param_mgr()->params();
    save_params(params, params_obj);
    primitive_object["param_rules"] = params_obj;

    QJsonObject rules_obj;
    auto rules = mp_pdk_instance->param_mgr()->rules();
    save_rules(rules, rules_obj);
    primitive_object["param_rules"] = rules_obj;

    root_object["primitive"] = primitive_object;
}

void JsonPaser::load_pcell_from_file(const QJsonObject &root_object)
{
    QJsonObject primitive_object = root_object["primitive"].toObject();

    QJsonArray layers_array = primitive_object["layers"].toArray();
    auto layers = mp_pdk_instance->layer_mgr()->get_layers();
    load_layers(layers_array, layers);

    QJsonObject param_manager = primitive_object["param_rules"].toObject();
    auto params = mp_pdk_instance->param_mgr()->params();
    auto rules = mp_pdk_instance->param_mgr()->rules();
    load_params(param_manager, params);
    load_rules(param_manager, rules);

    QJsonArray anchors_array = primitive_object["anchors"].toArray();
    QVector<PointInf> anchors;
    load_anchors(anchors_array, anchors);
}

void JsonPaser::save_pdk_layers(const QMap<QString, GdsLayerInfo*> &layer_infos, QJsonObject &layers)
{
    QJsonObject layerMap;

    for (auto it = layer_infos.begin(); it != layer_infos.end(); ++it)
    {
        QJsonArray layerArray;
        layerArray.append(it.value()->layer_num);
        layerArray.append(it.value()->data_type);
        layerArray.append(it.value()->layer_name);
        layerMap[it.key()] = layerArray;
    }
    layers = layerMap;
}

void JsonPaser::load_pdk_layers(const QJsonObject &layers, QMap<QString, GdsLayerInfo*> &layer_infos)
{

    for (auto it = layers.begin(); it != layers.end(); ++it)
    {
        QString layerName = it.key();
        QJsonArray layerValues = it.value().toArray();
        if (layerValues.size() == 3)
        {
            GdsLayerInfo *gds_layer_info = new GdsLayerInfo();
            gds_layer_info->layer_num = layerValues.at(0).toInt();
            gds_layer_info->data_type = layerValues.at(1).toInt();
            gds_layer_info->layer_name = layerValues.at(2).toString();
            layer_infos[layerName] = gds_layer_info;
        }
        // qDebug() << "Layer:" << layerName << "Values:" << layerValues;
    }
}

void JsonPaser::save_extra_to_file(const QJsonObject &root_object)
{
    QJsonObject pdks_object;

    QJsonObject layer_map_obj;
    auto gds_layers = mp_pdk_instance->gds_layer_info();
    save_pdk_layers(gds_layers, layer_map_obj);
    pdks_object["layer_map"] = layer_map_obj;

    QJsonObject params_obj;
    auto params = mp_pdk_instance->param_mgr()->params();
    save_params(params, params_obj);
    pdks_object["param_rules"] = params_obj;

    QJsonObject rules_obj;
    auto rules = mp_pdk_instance->param_mgr()->rules();
    save_rules(rules, rules_obj);
    pdks_object["param_rules"] = rules_obj;

    root_object["pdks"] = pdks_object;
}

void JsonPaser::load_extra_from_file(const QJsonObject &root_object)
{
    QJsonObject primitive_object = root_object["pdks"].toObject();
    QJsonObject layer_map_obj = primitive_object["layer_map"].toObject();
    auto &layer_map = mp_pdk_instance->gds_layer_info();
    load_pdk_layers(layer_map_obj, layer_map);

    QJsonObject param_manager = primitive_object["param_rules"].toObject();
    auto params = mp_pdk_instance->param_mgr()->params();
    auto rules = mp_pdk_instance->param_mgr()->rules();
    load_params(param_manager, params);
    load_rules(param_manager, rules);

}

}
