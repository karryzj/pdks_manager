#ifndef PDKINSTANCE_H
#define PDKINSTANCE_H

#include <QString>
#include <QMap>
#include "pointE.h"
#include "jsonPaser.h"

namespace ly
{
class LayerMgr;
}

namespace pm
{
class ParamMgr;
class Rule;
}

namespace pdk
{
struct PointInf;
struct GdsLayerInfo;

class PdkInstance
{
public:
    explicit PdkInstance(const QString &name);
    bool load();
    ly::LayerMgr *layer_mgr() const;
    pm::ParamMgr *param_mgr() const;
    void init_extra_param_mgr();
    QVector<PointInf> anchors() const;
    void set_anchors(const QVector<PointInf> anchors);
    PointInf coord_anchor() const;
    void set_coord_anchor(const PointInf coord_anchor);
    QMap<QString, GdsLayerInfo*>& gds_layer_info();
    void set_gds_layer_info(const QString &layer_name, GdsLayerInfo* gds_layer_info);
    void set_gds_layer_info(const QMap<QString, GdsLayerInfo *> &gds_layer_map);

    QString path() const;
    void set_path(const QString &path);

    // 保存加载图元
    int save_pcell_file(const QString &filename);
    int load_pcell_file(const QString &filename);
    int save_layer_json_file(const QString &filename);
    int load_layer_json_file(const QString &filename);
    int load_extra_file(const QString &filename);

    QString password() const;
    void set_password(const QString &password);

    QList<GdsLayerInfo *>& pdk_gds_layers();
    void set_pdk_gds_layers(const QList<GdsLayerInfo *> &pdk_gds_layers);
    void add_pdk_gds_layer(GdsLayerInfo *pdk_gds_layer);
    void remove_pdk_gds_layer(GdsLayerInfo *pdk_gds_layer);

    // QVector<pm::Rule> rules() const;
    // void set_rules(const QVector<pm::Rule> &rules);
    // void add_rule(const pm::Rule &rule);
    // void remove_rule(const pm::Rule &rule);
    // bool contains_empty_string();
    // void clear_empty_rule();

    QString pcell_thumbnail() const;

    pm::ParamMgr *get_extra_param_mgr() const;
    void set_extra_param_mgr(pm::ParamMgr *extra_param_mgr);

    QString pdk_path() const;
    void set_pdk_path(const QString &pdk_path);

    int load_py_json_file(const QString &filename);
    void reset_data();
    void reset_gds_layers();

    QPair<QString, QString> cell_relation() const;
    void set_cell_relation(const QString &cell_type, const QString &cell_name);
    void set_cell_type(const QString &cell_type);
    void set_cell_name(const QString &cell_name);

    pm::ParamMgr *get_total_param_mgr() const;
    void set_total_param_mgr(pm::ParamMgr *total_param_mgr);
    void init_total_param_mgr();
    void add_params();
    void add_rules();
    void add_extra_params();
    void add_extra_rules();

private:
    QString m_name;
    QString m_pcell_thumbnail;
    QString m_pcell_json;
    QString m_layer_json;
    QString m_password;
    QString m_path;
    QString m_pdk_path;
    QPair<QString, QString>         m_cell_relation;

    ly::LayerMgr                    *mp_layer_mgr = nullptr;
    pm::ParamMgr                    *mp_param_mgr = nullptr;
    QVector<PointInf>               m_anchors;
    PointInf                        m_coord_anchor;
    QMap<QString, GdsLayerInfo*>    m_pdk_layers_map;
    QList<GdsLayerInfo*>            m_pdk_gds_layers;
    // QVector<pm::Rule>               m_rules;
    pm::ParamMgr                    *mp_extra_param_mgr;
    pm::ParamMgr                    *mp_total_param_mgr;
};

}
#endif // PDKINSTANCE_H
