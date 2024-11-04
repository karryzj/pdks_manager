#ifndef PDKLAYERITEMWIDGET_H
#define PDKLAYERITEMWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>


namespace pdk
{
struct GdsLayerInfo;
class StyleFrame;

class PdkLayerItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PdkLayerItemWidget(GdsLayerInfo *pri_pdk_info, QWidget *parent = nullptr);
    ~PdkLayerItemWidget();
    GdsLayerInfo* get_result();

private:
    void setup_ui();
    void fill_data();

    StyleFrame      *mp_style_frame;
    QLineEdit       *mp_name_lineEdit;
    QLineEdit       *mp_layer_num_lineEdit;
    QLineEdit       *mp_data_type_lineEdit;
    QPushButton     *mp_modify_button;
    QPushButton     *mp_del_button;
    GdsLayerInfo    *mp_pri_pdk_info;

private slots:
    void modify_gds_info();
    void del_gds_info();
signals:
    void delete_layer_item(PdkLayerItemWidget *widget);
    void modify_layer_item(GdsLayerInfo *gds_layer);
};
}
#endif // PDKLAYERITEMWIDGET_H
