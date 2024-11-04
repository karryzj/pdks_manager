#ifndef LAYERSELECTWIDGET_H
#define LAYERSELECTWIDGET_H

#include <QWidget>
#include <QStringList>
#include <QListWidget>
#include <QStandardItemModel>
#include <QPushButton>

namespace pdk
{
struct GdsLayerInfo;
class PdkInstance;

class LayerSelectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LayerSelectWidget(PdkInstance *pdk_instance, QWidget *parent = nullptr);
    QMap<QString, GdsLayerInfo*> get_result();

    QStringList layer_list() const;
    void set_layer_list(const QStringList &layer_list);

    QStringList gds_list() const;
    void set_gds_list(const QStringList &gds_list);
    void FillData();
    void clear_view();

private:

    void SetupUi();

    PdkInstance         *mp_pdk_instance;
    QStringList         m_layer_list;
    QStringList         m_gds_list;
    QListWidget         *mp_list_widget;
signals:
};

}
#endif // LAYERSELECTWIDGET_H
