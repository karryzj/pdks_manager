#include <ConfigManager.h>
#include <cmath>

#include "viewport.h"
#include "common_defines.h"
#include "qdebug.h"

namespace cm
{

Viewport::Viewport(int w, int h, QRectF box)
    : m_w(w)
    , m_h(h)
    , m_target_box(box)
    , m_trans(QTransform())
    , m_default_zoom_step(0.2)
{
    set_box(m_target_box);
}

void cm::Viewport::set_size(int w, int h)
{
    m_w = w;
    m_h = h;
    set_box(m_target_box);
}

void Viewport::set_box(QRectF box, bool original)
{
    if(original)
        m_origin_box = box;

    m_target_box = box;

    // scale factor
    double f_x = 1.0 * box.width() / std::max(m_w, 1);
    double f_y = 1.0 * box.height() / std::max(m_h, 1);
    double f = std::max(f_x, f_y);
    if(f < 1e-13)
        f = 0.001;

    //translate
    double mx = double (box.right ()) + double (box.left ());
    double my = double (box.top ()) + double (box.bottom ());

    //  use only integer shift vectors. That enables a partial update of the image.
    double dx = floor (0.5 + (mx / f - double (m_w)) * 0.5);
    double dy = floor (0.5 + (my / f - double (m_h)) * 0.5);

    m_trans = QTransform::fromTranslate(-dx, dy).scale(1 / f, -1 / f);

    emit view_changed();
}

void Viewport::restore()
{
    set_box(m_origin_box);
}

void Viewport::fit(QRectF fit_box)
{
    if(fit_box.width() < m_origin_box.width() || fit_box.height() < m_origin_box.height())
        fit_box = m_origin_box;

    set_box(fit_box);
}

const QRectF& Viewport::box() const
{
    return m_target_box;
}

QRectF Viewport::map_from_scene(QRectF rect) const
{
    return m_trans.inverted().mapRect(rect);
}

QPointF Viewport::map_from_scene(QPointF point) const
{
    return m_trans.inverted().map(point);
}

double Viewport::map_from_scene(double distance)
{
    return std::abs(m_trans.inverted().m11()) * distance;
}

QPointF Viewport::map_to_scene(QPointF point) const
{
    return m_trans.map(point);
}

QRectF Viewport::map_to_scene(QRectF rect) const
{
    return m_trans.mapRect(rect);
}

QTransform Viewport::map_to_scene(QTransform trans) const
{
    /*QTransform apply = m_trans;
    apply.translate(trans.dx(), trans.dy()).scale(trans.m11(), trans.m22());
    return apply;*/

    QTransform apply_trans(m_trans.m11() * trans.m11(),
                           trans.m12() * m_trans.m22(),
                           trans.m21() * m_trans.m11(),
                           m_trans.m22() * trans.m22(),
                           m_trans.dx() + trans.dx() * m_trans.m11(),
                           m_trans.dy() + trans.dy() * m_trans.m22());
    return apply_trans;
}

double Viewport::map_to_scene(double distance)
{
    return std::abs(m_trans.m11()) * distance;
}

QPointF Viewport::box_center() const
{
    return QPointF(m_target_box.center().x(), m_target_box.y() - m_target_box.height() / 2.0);
}

QTransform &Viewport::trans()
{
    return m_trans;
}

QSize Viewport::size() const
{
    return QSize(m_w, m_h);
}

void Viewport::pan_left(float distance)
{
    move_by(QPointF(-distance, 0));
}

void Viewport::pan_right(float distance)
{
    move_by(QPointF(distance, 0));
}

void Viewport::pan_up(float distance)
{
    move_by(QPointF(0, distance));
}

void Viewport::pan_down(float distance)
{
    move_by(QPointF(0, -distance));
}

void Viewport::zoom_in(QPointF pos, unsigned int delta)
{
    zoom_by(m_default_zoom_step * (delta / 120.), pos);
}

void Viewport::zoom_out(QPointF pos, unsigned int delta)
{
    zoom_by(-m_default_zoom_step * (delta / 120.), pos);
}

void Viewport::zoom_by(float zoom_step, QPointF pos)
{

    if(pos == QPointF(0, 0))
        pos = box_center();

    double f;
    if (zoom_step < 0)
    {
        f = 1.0 / (1.0 + zoom_step);
    }
    else
    {
        f = 1.0 - zoom_step;
    }

    qDebug() << "f: " << f;

    qreal top_x = pos.x () - (pos.x () - m_target_box.x()) * f;
    qreal top_y = pos.y () - (pos.y () - m_target_box.y()) * f;
    qreal w = m_target_box.width() * f;
    qreal h = m_target_box.height() * f;
    QRectF new_box(top_x, top_y, w, h);

    if(w < CM_BOX_SCALE_MIN_VALUE ||
            w > CM_BOX_SCALE_MAX_VALUE ||
            h < CM_BOX_SCALE_MIN_VALUE ||
            h > CM_BOX_SCALE_MAX_VALUE)

    {
        return;
    }

    m_target_box = new_box;
    set_box(m_target_box);
}

void Viewport::move_by(QPointF move)
{
    qreal top_x = m_target_box.x() + m_target_box.width() * move.x();
    qreal top_y = m_target_box.y() + m_target_box.height() * move.y();
    m_target_box = QRectF(top_x, top_y, m_target_box.width(), m_target_box.height());
    set_box(m_target_box);
}

}

