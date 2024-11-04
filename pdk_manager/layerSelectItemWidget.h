#ifndef LAYERSELECTITEMWIDGET_H
#define LAYERSELECTITEMWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>

namespace pdk
{
class PdkInstance;

class CustomComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit CustomComboBox(QWidget *parent = nullptr);

protected:
    void showPopup() override;

signals:
    void custom_showPopup();
};

class LayerSelectItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LayerSelectItemWidget(PdkInstance *pdk_instance, const QString &layer, const QString &gds_layer,QWidget *parent = nullptr);
    ~LayerSelectItemWidget();

private:
    void setupUi();
    void fillData();

    PdkInstance     *mp_pdk_instance;
    QLineEdit       *mp_layer_lineEdit;
    CustomComboBox  *mp_gds_info_comboBox;

    QString         m_layer_name;
    QString         m_gds_layer;
    QStringList     m_gds_list;

private slots:
    void gds_layer_changed(const QString &text);
    void update_comboBox();
signals:
};
}

#endif // LAYERSELECTITEMWIDGET_H
