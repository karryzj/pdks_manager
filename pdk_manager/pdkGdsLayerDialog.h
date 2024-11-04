#ifndef PDKGDSLAYERDIALOG_H
#define PDKGDSLAYERDIALOG_H

#include <QDialog>
#include <QObject>
#include <QLineEdit>
#include <QDialogButtonBox>

namespace pdk
{
class StyleFrame;
struct GdsLayerInfo;

class PdkGdsLayerDialog : public QDialog
{
    Q_OBJECT
public:
    PdkGdsLayerDialog(GdsLayerInfo *gds_layer_info, QWidget *parent = nullptr);
    ~PdkGdsLayerDialog();

private:
    void setup_ui();
    void fill_data();

    QLineEdit           *mp_layername_line_edit;
    QLineEdit           *mp_layernum_line_edit;
    QLineEdit           *mp_datatype_line_edit;
    StyleFrame          *mp_style_frame;
    QDialogButtonBox    *mp_buttonBox;
    GdsLayerInfo        *mp_gds_layer_info;

private slots:
    void onAccepted();
    void onRejected();
};
}
#endif // PDKGDSLAYERDIALOG_H
