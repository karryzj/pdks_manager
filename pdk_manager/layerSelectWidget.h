#ifndef LAYERSELECTWIDGET_H
#define LAYERSELECTWIDGET_H

#include <QWidget>
#include <QStringList>
#include <QListWidget>
#include <QStandardItemModel>
#include <QPushButton>

namespace pdk
{

class LayerSelectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LayerSelectWidget(const QStringList &layer_list, const QStringList &gds_list, QWidget *parent = nullptr);

private:

    void SetupUi();
    void FillData();
    QStringList         m_layer_list;
    QStringList         m_gds_list;
    QListWidget         *mp_list_widget;
    // QStandardItemModel  *mp_model;
signals:
};

}
#endif // LAYERSELECTWIDGET_H
