#ifndef LAYLISTITEMWIDGET_H
#define LAYLISTITEMWIDGET_H

#include <QObject>
#include <QWidget>
#include <QCheckBox>
#include "layInfo.h"

namespace ly
{
class LayStyleFrame;

class LayListItemWidget final : public QWidget
{
    Q_OBJECT
public:
    explicit LayListItemWidget(const LayerInfo &style, const QString &text, QWidget *parent = nullptr);

    ~LayListItemWidget() override;
    LayerInfo get_current_layerinfo() const;
    int get_current_check_state() const;

private:
    LayStyleFrame *mp_frame;
    QCheckBox     *mp_layer_check;
private slots:
    void onStyleFrameChanged(const LayerInfo &style);
    void onLayerStateChanged(int state);

signals:
    void styleChanged(const LayerInfo &style);
    void layerStateChanged(const QString &layer, int state);
};
}

#endif // LAYLISTITEMWIDGET_H
