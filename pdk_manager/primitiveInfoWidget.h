#ifndef PRIMITIVEINFOWIDGET_H
#define PRIMITIVEINFOWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>

class PrimitiveInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PrimitiveInfoWidget(QWidget *parent = nullptr);

private:
    void setup_ui();
    void fill_data();

    QLabel      *mp_thumbnail_label;
    QLabel      *mp_coord_anchor_label;
    QListWidget *mp_params_listWidget;
    QListWidget *mp_anchors_listWidget;

signals:
};

#endif // PRIMITIVEINFOWIDGET_H
