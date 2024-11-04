#include <QJsonDocument>
#include <QDebug>
#include <QDir>
#include "crypto.h"
#include "jsonPaser.h"
#include "pdkInstance.h"
#include "layerMgr.h"
#include "paramMgr.h"
#include "gdsLayerInfo.h"

namespace pdk
{


PdkInstance::PdkInstance(const QString &name): m_name(name),
    mp_layer_mgr(new ly::LayerMgr()), mp_param_mgr(new pm::ParamMgr()), mp_extra_param_mgr(new pm::ParamMgr()),mp_total_param_mgr(new pm::ParamMgr()),
    m_pcell_thumbnail("thumbnail.png"), m_pcell_json("data.json"), m_password("ququan123456")
{

}

bool PdkInstance::load()
{
    return true;
}

ly::LayerMgr *PdkInstance::layer_mgr() const
{
    return mp_layer_mgr;
}

pm::ParamMgr *PdkInstance::param_mgr() const
{
    return mp_param_mgr;
}

void PdkInstance::init_extra_param_mgr()
{
    mp_extra_param_mgr->params() = mp_param_mgr->clone_params();
}

QVector<PointInf> PdkInstance::anchors() const
{
    return m_anchors;
}

void PdkInstance::set_anchors(const QVector<PointInf> anchors)
{
    m_anchors = anchors;
}

PointInf PdkInstance::coord_anchor() const
{
    return m_coord_anchor;
}

void PdkInstance::set_coord_anchor(const PointInf coord_anchor)
{
    m_coord_anchor = coord_anchor;
}

QMap<QString, GdsLayerInfo *>& PdkInstance::gds_layer_info()
{
    return m_pdk_layers_map;
}

void PdkInstance::set_gds_layer_info(const QString &layer_name, GdsLayerInfo *gds_layer_info)
{
    m_pdk_layers_map[layer_name] = gds_layer_info;
}

void PdkInstance::set_gds_layer_info(const QMap<QString, GdsLayerInfo *> &gds_layer_map)
{
    m_pdk_layers_map = gds_layer_map;
}

int PdkInstance::save_pcell_file(const QString &filename)
{
    QJsonObject root_object;

    JsonPaser filer(this);
    filer.save_pcell_to_file(root_object);

    QJsonDocument doc(root_object);
    cm::Crypto::encrypt_json_doc(doc, filename, m_password);
    // QFile file(filename);
    // if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    // {
    //     // HINT@leixunyong。如果走到这里来，说明是"\"和"/"出现了问题。
    //     QString error = file.errorString();
    //     qWarning("Couldn't open save file.");
    //     return -1;
    // }
    // else
    // {
    //     file.write(doc.toJson());
    //     file.close();
    // }
    return 0;
}

int PdkInstance::load_pcell_file(const QString &filename)
{
    QJsonDocument doc = cm::Crypto::decrypt_json_doc(filename, m_password);
    if (!doc.isObject())
    {
        qDebug() << "JSON document is not an object.";
        return -1;
    }

    QJsonObject jsonObject = doc.object();

    JsonPaser filer(this);
    filer.load_pcell_from_file(jsonObject);
    return 0;
}

int PdkInstance::save_layer_json_file(const QString &filename)
{
    QJsonObject root_object;

    JsonPaser filer(this);
    filer.save_gds_layers_to_root(m_pdk_gds_layers, root_object);

    QJsonDocument doc(root_object);
    // cm::Crypto::encrypt_json_doc(doc, filename, m_password);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QString error = file.errorString();
        qWarning("Couldn't open save file.");
        return -1;
    }
    else
    {
        file.write(doc.toJson());
        file.close();
    }
    return 0;
}

int PdkInstance::load_layer_json_file(const QString &filename)
{
    JsonPaser filer(this);
    QJsonObject jsonObject = filer.parse_json_root(filename);
    filer.load_pcell_from_file(jsonObject);
    return 0;
}

int PdkInstance::load_extra_file(const QString &filename)
{
    JsonPaser filer(this);
    QJsonObject jsonObject = filer.parse_json_root(filename);
    filer.load_extra_from_root(jsonObject);
    return 0;
}

QString PdkInstance::password() const
{
    return m_password;
}

void PdkInstance::set_password(const QString &password)
{
    m_password = password;
}

QList<GdsLayerInfo *>& PdkInstance::pdk_gds_layers()
{
    return m_pdk_gds_layers;
}

void PdkInstance::set_pdk_gds_layers(const QList<GdsLayerInfo *> &pdk_gds_layers)
{
    m_pdk_gds_layers = pdk_gds_layers;
}

void PdkInstance::add_pdk_gds_layer(GdsLayerInfo *pdk_gds_layer)
{
    auto it = std::find_if(m_pdk_gds_layers.begin(), m_pdk_gds_layers.end(), [pdk_gds_layer](auto layer)
    {
        return layer->layer_num == pdk_gds_layer->layer_num && layer->data_type == pdk_gds_layer->data_type;
    });
    if (it == m_pdk_gds_layers.end())
    {
        m_pdk_gds_layers.append(pdk_gds_layer);
    }
}

void PdkInstance::remove_pdk_gds_layer(GdsLayerInfo *pdk_gds_layer)
{
    auto it = std::find_if(m_pdk_gds_layers.begin(), m_pdk_gds_layers.end(), [pdk_gds_layer](auto layer)
    {
        return layer->layer_num == pdk_gds_layer->layer_num && layer->data_type == pdk_gds_layer->data_type;
    });
    if (it != m_pdk_gds_layers.end())
    {
        m_pdk_gds_layers.removeAll(pdk_gds_layer);
    }
}

#if 0
QVector<pm::Rule> PdkInstance::rules() const
{
    return m_rules;
}

void PdkInstance::set_rules(const QVector<pm::Rule> &rules)
{
    m_rules = rules;
}

void PdkInstance::add_rule(const pm::Rule &rule)
{
    m_rules.push_back(rule);
}

void PdkInstance::remove_rule(const pm::Rule &rule)
{
    auto it = std::find_if(m_rules.begin(), m_rules.end(), [&](const auto& tmp_rule)
    {
        return tmp_rule.to_str() == rule.to_str();
    });
    if(it != m_rules.end())
    {
        m_rules.erase(it);
    }
}

bool PdkInstance::contains_empty_string()
{
    for (auto it = m_rules.begin(); it != m_rules.end(); ++it)
    {
        if ((*it).to_str().isEmpty())
        {
            return true;
        }
    }
    return false;
}

void PdkInstance::clear_empty_rule()
{
    for (auto it = m_rules.begin(); it != m_rules.end(); ++it)
    {
        if ((*it).to_str().isEmpty())
        {
            m_rules.erase(it);
        }
    }
}
#endif

QString PdkInstance::pcell_thumbnail() const
{
    return m_pcell_thumbnail;
}

pm::ParamMgr *PdkInstance::get_extra_param_mgr() const
{
    return mp_extra_param_mgr;
}

void PdkInstance::set_extra_param_mgr(pm::ParamMgr *extra_param_mgr)
{
    mp_extra_param_mgr = extra_param_mgr;
}

QString PdkInstance::pdk_path() const
{
    return m_pdk_path;
}

void PdkInstance::set_pdk_path(const QString &pdk_path)
{
    m_pdk_path = pdk_path;
}

int PdkInstance::load_py_json_file(const QString &filename)
{
    JsonPaser filer(this);

    filer.load_python_json_file(filename);
    return 0;
}

void PdkInstance::reset_data()
{
    mp_layer_mgr->reset();
    mp_param_mgr->reset();
    mp_extra_param_mgr->reset();
    m_anchors.clear();
    // m_rules.clear();
    m_pdk_layers_map.clear();
}

void PdkInstance::reset_gds_layers()
{
    qDeleteAll(m_pdk_gds_layers);
    m_pdk_gds_layers.clear();
}

QPair<QString, QString> PdkInstance::cell_relation() const
{
    return m_cell_relation;
}

void PdkInstance::set_cell_relation(const QString &cell_type, const QString &cell_name)
{
    m_cell_relation.first = cell_type;
    m_cell_relation.second = cell_name;
}

void PdkInstance::set_cell_type(const QString &cell_type)
{
    m_cell_relation.first = cell_type;
}

void PdkInstance::set_cell_name(const QString &cell_name)
{
    m_cell_relation.second = cell_name;
}

pm::ParamMgr *PdkInstance::get_total_param_mgr() const
{
    return mp_total_param_mgr;
}

void PdkInstance::set_total_param_mgr(pm::ParamMgr *total_param_mgr)
{
    mp_total_param_mgr = total_param_mgr;
}

void PdkInstance::init_total_param_mgr()
{
    mp_total_param_mgr->params() = mp_extra_param_mgr->clone_params();
    mp_total_param_mgr->rules() = mp_extra_param_mgr->clone_rules();
    for (auto rule : mp_param_mgr->rules())
    {
        mp_total_param_mgr->rules().append(rule);
    }
}

QString PdkInstance::path() const
{
    return m_path;
}

void PdkInstance::set_path(const QString &path)
{
    m_path = path;
}

}
