#ifndef LAYWIDGET_H
#define LAYWIDGET_H

#include <QWidget>
#include <QMap>
#include <QPushButton>
#include <QListWidget>

#include "layerCommon.h"

namespace ly
{

class LayerInfo;
class LayerMgr;

class LY_PUBLIC LayerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LayerWidget(ly::LayerMgr *layer_mgr, QWidget *parent = nullptr);
    ~LayerWidget() override;
    const QListWidget* list_widget() const;
    const QList<LayerInfo*>& layer_infos(void) const;

    void update_all_items();
signals:
    void layer_changed();

private:
    void updateButtonState();
    int find_small_layer(const QList<LayerInfo*> &layers, const QString &prefix);

private slots:
    void addItems();
    void removeItem();
    void updateRemoveButtonState();
    void onItemStyleChanged(const LayerInfo &style);
    void onItemStateChanged(const QString &layer, int state);
private:
    LayerMgr    *mp_layer_mgr;
    QListWidget *mp_list_widget;
    QPushButton *mp_new_layer_button;
    QPushButton *mp_del_layer_button;
    // int m_visible_count;
    QList<LayerInfo*> &m_layers;

};

}
#endif // LAYWIDGET_H
