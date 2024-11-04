#ifndef JSONPASER_H
#define JSONPASER_H

#include <QJsonObject>
#include <QJsonArray>
#include "pointE.h"

namespace pm
{
class PointE;
class ParamDecl;
class Rule;
}

namespace ly
{
class LayerInfo;
}
namespace pdk
{

struct PointInf
{
    int id;
    int attach_point_idx;
    pm::PointE pos;
};

class PdkInstance;
struct GdsLayerInfo;

class JsonPaser
{
public:
    JsonPaser();
    explicit JsonPaser(PdkInstance* pdk_instance);
    ~JsonPaser();
    void save_pcell_to_file(QJsonObject &root_object);
    void load_pcell_from_file(const QJsonObject &root_object);
    void save_extra_to_root(QJsonObject &root_object);
    void load_extra_from_root(const QJsonObject &root_object);
    void save_extra_to_file(const QString &file_name);
    void load_extra_from_file(const QString &file_name);
    void save_gds_layers_to_root(const QList<GdsLayerInfo *> &gds_layers, QJsonObject &root_object);
    void load_gds_layers_from_root(const QJsonObject &root_object, QList<GdsLayerInfo *> &gds_layers);
    QJsonDocument parse_json_doc(const QString file_path);
    QJsonObject parse_json_root(const QString file_path);
    void save_gds_layers(const QList<GdsLayerInfo*> &gds_layers, QJsonArray &layers);
    void load_gds_layers(const QJsonArray &layers, QList<GdsLayerInfo*> &gds_layers);
    void save_json_doc(const QJsonDocument json_doc, const QString file_path);
    void save_json_root(const QJsonObject json_root, const QString file_path);
    int load_python_json_file(const QString &filename);

private:
    bool contains_layer(const QList<ly::LayerInfo*> &layer_infos, const QString &layer_name);
    pm::ParamDecl *json_obj_to_param(const QJsonObject &param_json_obj);
    QJsonObject param_to_json_obj(const pm::ParamDecl *param);

    void save_coord_anchor(PointInf point_inf, QJsonObject &coord_anchor_obj);
    void load_coord_anchor(const QJsonObject &coord_anchor_obj, PointInf &point_inf);

    void save_anchors(QVector<PointInf> anchors, QJsonArray &anchors_array);
    void load_anchors(const QJsonArray &anchors_array, QVector<PointInf> &anchors);

    void save_layers(const QList<ly::LayerInfo*> &layer_infos, QJsonArray &layers);
    void load_layers(const QJsonArray &layers, QList<ly::LayerInfo*> &layer_infos);

    void save_params(const QList<pm::ParamDecl*> &params, QJsonObject &param_rules);
    void load_params(const QJsonObject &param_rules, QList<pm::ParamDecl*> &params);

    void save_rules(const QVector<pm::Rule> &rules, QJsonObject &param_rules);
    void load_rules(const QJsonObject &param_rules, QVector<pm::Rule> &params);

    void save_pdk_layers(const QMap<QString, GdsLayerInfo*> &layer_infos, QJsonObject &layers);
    void load_pdk_layers(const QJsonObject &layers, QMap<QString, GdsLayerInfo*> &layer_infos);

    void save_pcell_relation(const QPair<QString, QString> &cell_info, QJsonObject &pcell_obj);
    void load_pcell_relation(const QJsonObject &pcell_obj, QString &cell_type, QString &cell_name);

    int load_py_layers(const QJsonArray &layers);
    void load_py_info_from_file(const QJsonObject &root_object);

    PdkInstance             *mp_pdk_instance;
    QList<GdsLayerInfo*>    m_gds_layers;
};
}

#endif // JSONPASER_H
