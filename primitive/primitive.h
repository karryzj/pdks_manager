#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <QObject>
#include "attachTreeUtils.h"

QT_BEGIN_NAMESPACE

class QGraphicsView;

QT_END_NAMESPACE

namespace pm
{
class ParamMgr;
class PointE;
}

namespace ly
{
class LayerMgr;
}

namespace cm
{

class Viewport;

}

namespace at
{
class AttachTreeRootNode;
class AttachTreeNode;
class AttachTreeNodeMgr;
class AttachTreeBaseNode;
class AttachTreeUtils;
}

namespace pr
{

class PrimitiveMgr;

class Primitive final : public QObject
{
    Q_OBJECT
public:
    friend class PrimitiveMgr;
    ~Primitive() override;
    QPair<QString, QString> primitive_path() const;
    QString name() const;
    ly::LayerMgr *layer_mgr() const;
    pm::ParamMgr *param_mgr() const;
    const QVector<at::AttachTreeUtils::AttachPointPosInf>& get_json_anchors();
    void set_json_anchors(const QVector<at::AttachTreeUtils::AttachPointPosInf> &anchors);  // 这里是缓存了锚点的记录

    at::AttachTreeNodeMgr* tree_node_mgr() const;
    at::AttachTreeRootNode *at_root() const;
    int save();
    int load();
    int load(QGraphicsView *view);
    QString thumbnail() const;
    void reset();
    double dbu();
    int load_py_json();

    double arc_len() const;
    void set_arc_len(double arc_len);

private:
    // for load
    Primitive(QString name, double dbu = 0.01);
    // for new
    Primitive(QString name, QGraphicsView *view, double dbu = 0.01);

    // 保存加载图元
    int save_json_file(const QString &filename);
    int load_json_file(const QString &filename);
    int load_base_json_file(const QString &filename);
    int load_node_json_file(const QString &filename);
    int load_python_json_file(const QString &filename);

signals:
    void signal_add_new_tree();

private:
    QString m_name;
    QString m_thumbnail;
    QString m_json;
    QString m_py_json;
    QString m_password;
    double  m_dbu;
    double  m_arc_len;

    ly::LayerMgr *mp_layer_mgr = nullptr;
    pm::ParamMgr *mp_param_mgr = nullptr;
    QVector<at::AttachTreeUtils::AttachPointPosInf> m_json_anchors;

    at::AttachTreeNodeMgr* mp_tree_node_mgr = nullptr;
    at::AttachTreeRootNode *mp_at_root = nullptr;
};

}

#endif // PRIMITIVE_H
