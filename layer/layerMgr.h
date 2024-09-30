#ifndef LAYERMGR_H
#define LAYERMGR_H

#include <QWidget>
#include <QPushButton>
#include <QListWidget>

#include "layerCommon.h"

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

namespace ly
{

class LayerInfo;

class LY_PUBLIC LayerMgr final
{
public:
    explicit LayerMgr();
    ~LayerMgr();

    LayerInfo* get_layer_info_by_name(const QString &lay_name) const;
    void set_layers(LayerInfo *lay_info);
    QList<LayerInfo*>& get_layers();
    // QList<LayerInfo> getLayers() const;
    void getPainter(QPainter &painter, const LayerInfo &lay_info);
    void remove_layer_info(const LayerInfo &lay_info);
    void reset();
private:
    QList<LayerInfo*> m_layers;
    bool find_layer_info_by_name(const QString &lay_name);
};

}

#endif // LAYERMGR_H

