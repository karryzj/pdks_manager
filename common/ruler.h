#ifndef RULER_H
#define RULER_H

#include <QObject>
#include <QGraphicsItem>
#include <QSize>

#include "common.h"

#define R_WIDTH_PROPOSAL 60
#define R_HEIGHT_PROPOSAL 18
#define R_TEXT_HEIGHT 6
#define R_CORNER_SPACE 100


namespace cm
{

class Viewport;

class CM_PUBLIC Ruler : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    Ruler(bool nano = false /* default micron **/);
    ~Ruler();

    void num_options(std::vector<int> num_options);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override final;
    QRectF boundingRect() const override final;

    void set(double dbu, Viewport *viewport);

    double val();
    double micron();
    double nano();
    double width();

private:

    void paint_grid(QPainter *painter);
    void paint_ruler(QPainter *painter);

    // to one num
    double f(double d);
    //find closest num
    double option(double opt);

private:
    bool m_nano;
    QPointF m_pos;
    std::vector<int> m_num_options;


    //calc ret
    double m_micron;
    double m_width;
    double m_height;

    Viewport *mp_viewport;
};

}

#endif // RULER_H
