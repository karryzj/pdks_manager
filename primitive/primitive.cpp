#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QMap>
#include <QVector>
#include <QGraphicsView>
#include "primitive.h"
#include "paramMgr.h"
#include "paramDecl.h"
#include "rule.h"
#include "layerMgr.h"
#include "attachTreeRootNode.h"
#include "attachTreeNode.h"
#include "scopeTimer.h"
#include "shapeBase.h"
#include <PriAttachTreeNodeMgr.h>
#include <QDebug>
#include "attachTreeNode.h"
#include "priFiler.h"
#include "crypto.h"

namespace pr
{

Primitive::Primitive(QString name, double dbu)
    : QObject()
    , m_name(name)
    , m_thumbnail(name + "/" + "thumbnail.png")
    , m_json(name + "/" + "data.json")
    , m_dbu(dbu)
    , m_password("ququan123456")
{

}

Primitive::Primitive(QString name, QGraphicsView *view, double dbu)
    : QObject()
    , m_name(name)
    , m_thumbnail(name + "/" + "thumbnail.png")
    , m_json(name + "/" + "data.json")
    , m_dbu(dbu)
    , m_password("ququan123456")
    , mp_layer_mgr(new ly::LayerMgr())
    , mp_param_mgr(new pm::ParamMgr())
    , mp_tree_node_mgr(new PriAttachTreeNodeMgr())
{
    mp_tree_node_mgr->init_viewport(m_dbu);
    mp_at_root = new at::AttachTreeRootNode(mp_param_mgr, view, mp_tree_node_mgr);
    mp_tree_node_mgr->update();
}

int Primitive::save()
{
    QString full_path;

    if (m_json.contains("/primitives"))
    {
        full_path = m_json;
    }
    else
    {
        QString current_path = QDir::currentPath() + "/data/primitives";
        QString json_path = m_json.replace("#", "/");
        full_path = current_path + "/" + json_path;
    }

    return save_json_file(full_path);
}

int Primitive::load()
{
    if(!mp_layer_mgr)
    {
        mp_layer_mgr = new ly::LayerMgr();
    }
    else
    {
        mp_layer_mgr->reset();
    }

    if(!mp_param_mgr)
    {
        mp_param_mgr = new pm::ParamMgr();
    }
    else
    {
        mp_param_mgr->reset();
    }

    QString full_path;
    if (m_json.contains("/primitives"))
    {
        full_path = m_json;
    }
    else
    {
        QString current_path = QDir::currentPath() + "/data/primitives";
        QString json_path = m_json.replace("#", "/");
        full_path = current_path + "/" + json_path;
    }

    int ret = load_base_json_file(full_path);
    emit signal_add_new_tree();
    return ret;
}

int Primitive::load_base_json_file(const QString &filename)
{
    // QFile file(filename);
    // if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    // {
    //     qDebug() << "Failed to open file:" << filename;
    //     return -1;
    // }
    // QByteArray jsonData = file.readAll();
    // file.close();

    // QJsonParseError parseError;
    // QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    // if (parseError.error != QJsonParseError::NoError)
    // {
    //     qDebug() << "Failed to parse JSON:" << parseError.errorString();
    //     return -1;
    // }

    QJsonDocument doc = cm::Crypto::decrypt_json_doc(filename, m_password);
    if (!doc.isObject())
    {
        qDebug() << "JSON document is not an object.";
        return -1;
    }

    QJsonObject jsonObject = doc.object();

    PriFiler filer(this);
    filer.load_base_info_from_file(jsonObject);
    emit signal_add_new_tree();
    return 0;
}

int Primitive::load_node_json_file(const QString &filename)
{
    // QFile file(filename);
    // if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    // {
    //     qDebug() << "Failed to open file:" << filename;
    //     return -1;
    // }

    // QByteArray jsonData = file.readAll();
    // file.close();

    // QJsonParseError parseError;
    // QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    // if (parseError.error != QJsonParseError::NoError)
    // {
    //     qDebug() << "Failed to parse JSON:" << parseError.errorString();
    //     return -1;
    // }

    QJsonDocument doc = cm::Crypto::decrypt_json_doc(filename, m_password);
    if (!doc.isObject())
    {
        qDebug() << "JSON document is not an object.";
        return -1;
    }

    QJsonObject jsonObject = doc.object();

    PriFiler filer(this);
    filer.load_node_from_file(jsonObject);
    emit signal_add_new_tree();
    return 0;
}

void Primitive::set_json_anchors(const QVector<at::AttachTreeUtils::AttachPointPosInf> &anchors)
{
    m_json_anchors = anchors;
}

void Primitive::set_anchors(const QVector<at::AttachTreeUtils::AttachPointPosInf> &anchors)
{
    at::AttachTreeUtils::set_anchors_inf(at_root(), anchors);
}

int Primitive::load(QGraphicsView *view)
{
    SCOPE_TIMER("Primitive::load");
    if(!mp_tree_node_mgr)
    {
        mp_tree_node_mgr = new PriAttachTreeNodeMgr();
    }

    mp_tree_node_mgr->set_disable(true);

    if(!mp_at_root)
    {
        mp_at_root = new at::AttachTreeRootNode(mp_param_mgr, nullptr, mp_tree_node_mgr);
    }
    else
    {
        mp_at_root->remove_all();
        mp_at_root->set_new_graphics_view(nullptr);
    }

    mp_tree_node_mgr->init_viewport(m_dbu);

    QString full_path;
    if (m_json.contains("/primitives"))
    {
        full_path = m_json;
    }
    else
    {
        QString current_path = QDir::currentPath() + "/data/primitives";
        QString json_path = m_json.replace("#", "/");
        full_path = current_path + "/" + json_path;
    }

    mp_tree_node_mgr->init_viewport(m_dbu);
    int ret = load_node_json_file(full_path);

    mp_tree_node_mgr->set_disable(false);

    mp_at_root->set_new_graphics_view(view);
    at::AttachTreeUtils::update_all_nodes_data(mp_at_root);
    mp_tree_node_mgr->update();
    return ret;
}

QString Primitive::thumbnail() const
{
    return m_thumbnail;
}

void Primitive::reset()
{
    mp_layer_mgr->reset();
    mp_param_mgr->reset();
}

double Primitive::dbu()
{
    return m_dbu;
}

int Primitive::save_json_file(const QString &filename)
{
    QJsonObject root_object;

    PriFiler filer(this);
    filer.save_primitive_to_file(root_object);

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

Primitive::~Primitive()
{
    delete mp_at_root;
    delete mp_param_mgr;
    delete mp_layer_mgr;
}

QPair<QString, QString> pr::Primitive::primitive_path() const
{
    // 使用 split 方法分割字符串
    QString path = m_name + '/';

    // 使用 QFileInfo 获取路径信息
    QFileInfo fileInfo(path);

    // 获取目录路径
    QDir dir = fileInfo.dir();

    // 获取末尾的一级文件夹目录
    QString lastFolderName = dir.dirName();

    // 获取末尾的二级文件夹目录
    dir.cdUp(); // 上一级目录
    QString secondLastFolderName = dir.dirName();

    return QPair<QString, QString>(secondLastFolderName, lastFolderName);
}

QString Primitive::name() const
{
    return m_name;
}

ly::LayerMgr *Primitive::layer_mgr() const
{
    return mp_layer_mgr;
}

pm::ParamMgr *Primitive::param_mgr() const
{
    return mp_param_mgr;
}

QVector<at::AttachTreeUtils::AttachPointPosInf> Primitive::get_json_anchors()
{
    return m_json_anchors;
}

at::AttachTreeNodeMgr *Primitive::tree_node_mgr() const
{
    return mp_tree_node_mgr;
}

at::AttachTreeRootNode *Primitive::at_root() const
{
    return mp_at_root;
}


}
