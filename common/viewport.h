#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "QObject"
#include <QTransform>
#include <QRectF>
#include "common.h"

namespace cm
{

class CM_PUBLIC Viewport final : public QObject
{
    Q_OBJECT
public:
    Viewport(int w = 10, int h = 10, QRectF box = QRectF(-1, 1, 2, 2));

    // global size.
    void set_size(int w, int h);
    // target box to show.
    void set_box(QRectF box, bool original = false);
    // set target box to original box
    void restore();
    // fit to
    void fit(QRectF fit_box);

    const QRectF& box() const;
    // scene coord to min
    QRectF map_from_scene(QRectF rect) const;
    // scene coord to mine
    QPointF map_from_scene(QPointF point) const;
    // scene coord to mine
    double map_from_scene(double distance);
    // my coord to scene
    QPointF map_to_scene(QPointF point) const;
    // my coord to scene
    QRectF map_to_scene(QRectF rect) const;
    // my coord to scene
    QTransform map_to_scene(QTransform trans) const;
    // my coord to scene
    double map_to_scene(double distance);


    //target center
    QPointF box_center() const;

    QTransform &trans();
    QSize size() const;

    //mouse operation.
    void pan_left(float distance = 0.15);
    void pan_right(float distance = 0.15);
    void pan_up(float distance = 0.15);
    void pan_down(float distance = 0.15);
    // 放大
    void zoom_in(QPointF pos = QPointF(0, 0)/**默认中心点*/, unsigned int delta = 120);
    // 缩小
    void zoom_out(QPointF pos = QPointF(0, 0)/**默认中心点*/, unsigned int delta = 120);

private:

    void move_by(QPointF move);
    void zoom_by(float zoom_step /**zoom_out>0  zoom_in<0*/, QPointF pos = QPointF(0, 0) /**默认中心点*/);
signals:
    void view_changed();

private:
    int m_w;
    int m_h;
    QRectF m_target_box;
    QRectF m_origin_box;
    QTransform m_trans;
    double m_default_zoom_step;
};


}




#endif // VIEWPORT_H
