#include <QFile>
#include <QJsonDocument>
#include "pointE.h"
#include "layInfo.h"
#include "paramDecl.h"
#include "rule.h"
#include "jsonPaser.h"
#include "pdkInstance.h"
#include "layerMgr.h"
#include "paramMgr.h"
#include "gdsLayerInfo.h"

namespace pdk
{
JsonPaser::JsonPaser()
{

}

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
        rules.push_back(rule);
    }
}

void JsonPaser::save_pcell_to_file(QJsonObject &root_object)
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
    QList<ly::LayerInfo*> layers;
    load_layers(layers_array, layers);
    for(auto layer : layers)
    {
        mp_pdk_instance->layer_mgr()->set_layers(layer);
    }

    QJsonObject param_manager = primitive_object["param_rules"].toObject();
    auto &params = mp_pdk_instance->param_mgr()->params();
    auto &rules = mp_pdk_instance->param_mgr()->rules();
    load_params(param_manager, params);
    load_rules(param_manager, rules);

    QJsonArray anchors_array = primitive_object["anchors"].toArray();
    QVector<PointInf> anchors;
    load_anchors(anchors_array, anchors);
    mp_pdk_instance->set_anchors(anchors);

    QJsonObject coord_anchor_obj = primitive_object["coord_anchor"].toObject();
    PointInf coord_anchor;
    load_coord_anchor(coord_anchor_obj, coord_anchor);
    mp_pdk_instance->set_coord_anchor(coord_anchor);
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

void JsonPaser::save_pcell_relation(const QPair<QString, QString> &cell_info, QJsonObject &pcell_obj)
{
    pcell_obj["cell_type"] = cell_info.first;
    pcell_obj["cell_name"] = cell_info.second;
}

void JsonPaser::load_pcell_relation(const QJsonObject &pcell_obj, QString &cell_type, QString &cell_name)
{
    cell_type = pcell_obj["cell_type"].toString();
    cell_name = pcell_obj["cell_name"].toString();
}

void JsonPaser::save_extra_to_root(QJsonObject &root_object)
{
    QJsonObject pdks_object;

    QJsonObject pcell_obj;
    save_pcell_relation(mp_pdk_instance->cell_relation(), pcell_obj);
    pdks_object["pcell"] = pcell_obj;

    QJsonObject layer_map_obj;
    auto gds_layers = mp_pdk_instance->gds_layer_info();
    save_pdk_layers(gds_layers, layer_map_obj);
    pdks_object["layer_map"] = layer_map_obj;

    QJsonObject param_rules_obj;
    auto params = mp_pdk_instance->get_extra_param_mgr()->params();
    save_params(params, param_rules_obj);
    // pdks_object["param_rules"]. = params_obj;

    QJsonObject rules_obj;
    auto rules = mp_pdk_instance->get_extra_param_mgr()->rules();
    save_rules(rules, param_rules_obj);
    pdks_object["param_rules"] = param_rules_obj;

    root_object["pdks"] = pdks_object;
}

void JsonPaser::load_extra_from_root(const QJsonObject &root_object)
{
    QJsonObject primitive_object = root_object["pdks"].toObject();

    QJsonObject pcell_obj = primitive_object["pcell"].toObject();
    QString cell_type, cell_name;
    load_pcell_relation(pcell_obj, cell_type, cell_name);
    mp_pdk_instance->set_cell_relation(cell_type, cell_name);

    QJsonObject layer_map_obj = primitive_object["layer_map"].toObject();
    auto &layer_map = mp_pdk_instance->gds_layer_info();
    load_pdk_layers(layer_map_obj, layer_map);

    QJsonObject param_manager = primitive_object["param_rules"].toObject();
    auto &params = mp_pdk_instance->get_extra_param_mgr()->params();
    auto &rules = mp_pdk_instance->get_extra_param_mgr()->rules();
    load_params(param_manager, params);
    load_rules(param_manager, rules);

}

void JsonPaser::save_extra_to_file(const QString &file_name)
{
    QJsonObject root_object;
    save_extra_to_root(root_object);
    save_json_root(root_object, file_name);
}

void JsonPaser::load_extra_from_file(const QString &file_name)
{
    QJsonObject root_object;
    root_object = parse_json_root(file_name);
    load_extra_from_root(root_object);
}

void JsonPaser::save_gds_layers(const QList<GdsLayerInfo *> &gds_layers, QJsonArray &layers_array)
{
    for (const GdsLayerInfo *layerInfo : gds_layers)
    {
        QJsonObject layerObj;
        layerObj["layer_name"] = layerInfo->layer_name;
        layerObj["layer_num"] = layerInfo->layer_num;
        layerObj["data_type"] = layerInfo->data_type;

        QJsonObject styleObj;
        styleObj["border_color"] = layerInfo->border_style.border_color;
        styleObj["border_line"] = layerInfo->border_style.border_line;
        styleObj["fill_color"] = layerInfo->border_style.fill_color;
        styleObj["fill_line"] = layerInfo->border_style.fill_line;

        layerObj["style"] = styleObj;
        layers_array.append(layerObj);
    }
}

void JsonPaser::load_gds_layers(const QJsonArray &layers_array, QList<GdsLayerInfo *> &gds_layers)
{
    for (const QJsonValue &value : layers_array)
    {
        QJsonObject layer_object = value.toObject();
        GdsLayerInfo *layerInfo = new GdsLayerInfo;
        layerInfo->layer_name = layer_object.value("layer_name").toString();
        layerInfo->layer_num = layer_object.value("layer_num").toInt();
        layerInfo->data_type = layer_object.value("data_type").toInt();

        QJsonObject style_obj = layer_object.value("style").toObject();
        layerInfo->border_style.border_color = style_obj.value("border_color").toInt();
        layerInfo->border_style.border_line = style_obj.value("border_line").toInt();
        layerInfo->border_style.fill_color = style_obj.value("fill_color").toInt();
        layerInfo->border_style.fill_line = style_obj.value("fill_line").toInt();

        gds_layers.append(layerInfo);
    }
}

void JsonPaser::save_gds_layers_to_root(const QList<GdsLayerInfo *> &gds_layers, QJsonObject &root_object)
{
    QJsonArray layers_array;

    save_gds_layers(gds_layers, layers_array);
    root_object["layers"] = layers_array;
}

void JsonPaser::load_gds_layers_from_root(const QJsonObject &root_object, QList<GdsLayerInfo *> &gds_layers)
{
    QJsonArray layers_array = root_object.value("layers").toArray();
    load_gds_layers(layers_array, gds_layers);

}

void JsonPaser::save_json_doc(const QJsonDocument json_doc, const QString file_path)
{
    QByteArray jsonData = json_doc.toJson();

    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    file.write(jsonData);
    file.close();
}

void JsonPaser::save_json_root(const QJsonObject json_root, const QString file_path)
{
    QJsonDocument document(json_root);
    save_json_doc(document, file_path);
}

int JsonPaser::load_python_json_file(const QString &filename)
{
    QJsonObject root_obj = parse_json_root(filename);
    load_py_info_from_file(root_obj);
    return 0;
}

QJsonDocument JsonPaser::parse_json_doc(const QString file_path)
{
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return QJsonDocument();
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject())
    {
        return QJsonDocument();
    }
    return document;
}

QJsonObject JsonPaser::parse_json_root(const QString file_path)
{
    QJsonDocument document = parse_json_doc(file_path);
    if (document.isNull() || !document.isObject())
    {
        return QJsonObject();
    }
    QJsonObject root_obj = document.object();
    return root_obj;
}

int JsonPaser::load_py_layers(const QJsonArray &layers)
{
    for (int i = 0; i < layers.size(); ++i)
    {
        auto layer_name = layers[i].toString();
        // auto layer_name = layer_object.toString();
        // 这里需要额外进行查重判断。如果已经有同名的图层就不进行加载。
        if(mp_pdk_instance->layer_mgr()->get_layer_info_by_name(layer_name))
        {
            continue;
        }

        ly::LayerInfo *layer_info = new ly::LayerInfo();
        layer_info->set_layer_name(layer_name);
        layer_info->set_border_line(ly::LayerInfo::Border_Style::SolidLine);
        layer_info->set_border_color(ly::LayerInfo::Layer_Color::Color1);
        layer_info->set_fill_line(ly::LayerInfo::Fill_Style::NotFill);
        layer_info->set_fill_color(ly::LayerInfo::Layer_Color::Color1);
        mp_pdk_instance->layer_mgr()->set_layers(layer_info);
    }
    return 0;
}

void JsonPaser::load_py_info_from_file(const QJsonObject &root_object)
{
    QJsonArray layers = root_object["layers"].toArray();
    load_py_layers(layers);

    QJsonObject param_manager = root_object["param_rules"].toObject();
    auto &params = mp_pdk_instance->param_mgr()->params();
    auto &rules = mp_pdk_instance->param_mgr()->rules();
    load_params(param_manager, params);
    load_rules(param_manager, rules);

    QJsonArray anchors_array = root_object["anchors"].toArray();
    QVector<PointInf> anchors;
    load_anchors(anchors_array, anchors);
    mp_pdk_instance->set_anchors(anchors);
}

}
