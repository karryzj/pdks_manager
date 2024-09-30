#ifndef LAYERSELECTITEMWIDGET_H
#define LAYERSELECTITEMWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>

namespace pdk
{
class LayerSelectItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LayerSelectItemWidget(const QString &layer, const QStringList &gds_list,QWidget *parent = nullptr);
    ~LayerSelectItemWidget();

private:
    void setupUi();
    void fillData();

    QLineEdit       *mp_layer_lineEdit;
    QComboBox       *mp_gds_info_comboBox;

    QString         m_layer_name;
    QStringList     m_gds_list;
signals:
};
}

#endif // LAYERSELECTITEMWIDGET_H
