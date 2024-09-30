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
    explicit JsonPaser(PdkInstance* pdk_instance);
    ~JsonPaser();
    void save_pcell_to_file(const QJsonObject &root_object);
    void load_pcell_from_file(const QJsonObject &root_object);
    void save_extra_to_file(const QJsonObject &root_object);
    void load_extra_from_file(const QJsonObject &root_object);
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

    PdkInstance* mp_pdk_instance;
};
}

#endif // JSONPASER_H
