#include <QJsonDocument>
#include <QDebug>
#include <QDir>
#include "crypto.h"
#include "jsonPaser.h"
#include "pdkInstance.h"
#include "layerMgr.h"
#include "paramMgr.h"

namespace pdk
{


PdkInstance::PdkInstance(const QString &name): m_name(name),
    mp_layer_mgr(new ly::LayerMgr()), mp_param_mgr(new pm::ParamMgr())
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
    filer.save_pcell_to_file(root_object);

    QJsonDocument doc(root_object);
    // cm::Crypto::encrypt_json_doc(doc, filename, m_password);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // HINT@leixunyong。如果走到这里来，说明是"\"和"/"出现了问题。
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
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file:" << filename;
        return -1;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError)
    {
        qDebug() << "Failed to parse JSON:" << parseError.errorString();
        return -1;
    }

    // QJsonDocument doc = cm::Crypto::decrypt_json_doc(filename, m_password);
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

}
