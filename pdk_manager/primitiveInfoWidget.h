#ifndef PRIMITIVEINFOWIDGET_H
#define PRIMITIVEINFOWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>

namespace pdk
{
class PdkInstance;
class PrimitiveInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PrimitiveInfoWidget(PdkInstance *pdk_instance, QWidget *parent = nullptr);
    void fill_data();
    void clear_view();

private:
    void setup_ui();
    PdkInstance *mp_pdk_instance;
    QLabel      *mp_thumbnail_label;
    QLabel      *mp_coord_anchor_label;
    QListWidget *mp_params_listWidget;
    QListWidget *mp_anchors_listWidget;

signals:
};
}
#endif // PRIMITIVEINFOWIDGET_H
