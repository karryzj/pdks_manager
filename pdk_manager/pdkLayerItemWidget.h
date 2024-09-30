#ifndef PDKLAYERITEMWIDGET_H
#define PDKLAYERITEMWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>


namespace pdk
{
struct GdsLayerInfo;

class PdkLayerItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PdkLayerItemWidget(GdsLayerInfo *pri_pdk_info, QWidget *parent = nullptr);

private:
    void setup_ui();
    void fill_data();

    QLineEdit       *mp_name_lineEdit;
    QLineEdit       *mp_layer_num_lineEdit;
    QLineEdit       *mp_data_type_lineEdit;
    QPushButton     *mp_modify_button;
    QPushButton     *mp_del_button;

    GdsLayerInfo      *mp_pri_pdk_info;
signals:
};
}
#endif // PDKLAYERITEMWIDGET_H
