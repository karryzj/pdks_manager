#include <QPen>
#include <QBrush>
#include <QPainter>

#include "ruler.h"
#include "viewport.h"

namespace cm
{

Ruler::Ruler(bool nano)
    : QObject()
    , QGraphicsItem(nullptr)
    , m_nano(nano)
    , m_pos(QPointF(0.0, 0.0))
    , m_num_options({1, 2, 4, 10})
    , m_micron(1.0)
    , m_width(R_WIDTH_PROPOSAL)
    , m_height(R_HEIGHT_PROPOSAL)
    , mp_viewport(nullptr)
{

}

Ruler::~Ruler()
{

}

void Ruler::num_options(std::vector<int> num_options)
{
    m_num_options = num_options;
}

void Ruler::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    paint_grid(painter);
    paint_ruler(painter);

}

void Ruler::paint_grid(QPainter *painter)
{
    double grid = m_width / 2;

    QPointF org_point_view = mp_viewport->map_to_scene(QPointF(0,0));
    QPen pen = QPen(QColor("#cbcccc"), 0.5, Qt::DotLine);
    painter->setPen(pen);
    // to left
    double left = org_point_view.x();
    while(left >= 0)
    {
        painter->drawLine(QPoint(left,0), QPointF(left, mp_viewport->size().height()));
        left -= grid;
    }
    // to right
    double right = org_point_view.x() + grid;
    while(right <= mp_viewport->size().width())
    {
        painter->drawLine(QPoint(right,0), QPointF(right, mp_viewport->size().height()));
        right += grid;
    }
    // to top
    double top = org_point_view.y();
    while(top >= 0)
    {
        painter->drawLine(QPoint(0,top), QPointF(mp_viewport->size().width(), top));
        top -= grid;
    }
    // to down
    double down = org_point_view.y() + grid;
    while(down <= mp_viewport->size().height())
    {
        painter->drawLine(QPoint(0,down), QPointF(mp_viewport->size().width(), down));
        down += grid;
    }
}

void Ruler::paint_ruler(QPainter *painter)
{
    QPen pen = QPen(QColor("#b1b2b2"), 1, Qt::SolidLine);
    QBrush brush(QColor("#b1b2b2"));

    //rect
    painter->setPen(pen);
    painter->drawRect(QRectF(m_pos.x(), m_pos.y(), m_width, m_height));

    //half fill
    painter->setBrush(brush);
    painter->drawRect(QRectF(m_pos.x(), m_pos.y(), m_width / 2, m_height));

    //text above
    QString str;
    if(m_nano)
    {
        str = QString("%1nm").arg(nano());
    }
    else
    {
        str = QString("%1μm").arg(micron());
    }
    painter->drawText(m_pos.x(), m_pos.y() - R_TEXT_HEIGHT, str);
}

QRectF Ruler::boundingRect() const
{
    return mp_viewport ? QRectF(QPointF(0, 0), mp_viewport->size()) : QRectF();
}

void Ruler::set(double dbu, Viewport* viewport)
{
    mp_viewport = viewport;

    // pos it in the left-down corner
    m_pos = QPointF(R_CORNER_SPACE, viewport->size().height() - R_CORNER_SPACE);

    double l_w = viewport->map_from_scene(R_WIDTH_PROPOSAL);
    double macro = l_w;

    double opt_f = f(macro);
    double opt_try = macro * opt_f;
    double opt = option(opt_try);

    m_micron = opt * 1 / opt_f;
    m_width = viewport->map_to_scene(m_micron);
    m_height = 1.0 * R_HEIGHT_PROPOSAL / R_WIDTH_PROPOSAL * m_width;

    prepareGeometryChange();
    update();
}

double Ruler::val()
{
    return m_nano ?  nano() : micron();
}

double Ruler::micron()
{
    return m_micron;
}

double Ruler::nano()
{
    return m_micron * 1000.0;
}

double Ruler::width()
{
    return m_width;
}


double Ruler::f(double d)
{
    double f = 1;
    while(d < 1)
    {
        d *=10;
        f *= 10;
    }

    while(d > 10)
    {
        d /=10;
        f /=10;
    }

    return f;
}

double Ruler::option(double opt)
{
    int bf = 0;
    for(int num : m_num_options)
    {
        if(opt > num)
        {
            bf = num;
            continue;
        }
        else
        {
            return opt - bf < num - opt ? bf : num;
        }


    }

    return 1;
}


}
