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
}

namespace pdk
{
struct PointInf;
struct GdsLayerInfo
{
    int layer_num;
    int data_type;
    QString layer_name;
};

class PdkInstance
{
public:
    explicit PdkInstance(const QString &name);
    bool load();
    ly::LayerMgr *layer_mgr() const;
    pm::ParamMgr *param_mgr() const;
    QVector<PointInf> anchors() const;
    void set_anchors(const QVector<PointInf> anchors);
    PointInf coord_anchor() const;
    void set_coord_anchor(const PointInf coord_anchor);
    QMap<QString, GdsLayerInfo*>& gds_layer_info();
    void set_gds_layer_info(const QString &layer_name, GdsLayerInfo* gds_layer_info);
    void set_gds_layer_info(const QMap<QString, GdsLayerInfo *> &gds_layer_map);

private:
    // 保存加载图元
    int save_pcell_file(const QString &filename);
    int load_pcell_file(const QString &filename);
    int save_layer_json_file(const QString &filename);
    int load_layer_json_file(const QString &filename);

    QString m_name;
    QString m_pcell_thumbnail;
    QString m_pcell_json;
    QString m_layer_json;
    QString m_password;

    ly::LayerMgr                    *mp_layer_mgr = nullptr;
    pm::ParamMgr                    *mp_param_mgr = nullptr;
    QVector<PointInf>               m_anchors;
    PointInf                        m_coord_anchor;
    QMap<QString, GdsLayerInfo*>    m_pdk_layers_map;
};

}
#endif // PDKINSTANCE_H
