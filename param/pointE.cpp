#include "pointE.h"
#include "transformE.h"

namespace pm
{


PointE::PointE(const pm::Expression &exp_x, const pm::Expression &exp_y)
    : QObject()
    , m_exp_x(exp_x)
    , m_exp_y(exp_y)
{

}

PointE::PointE(const std::string &exp_x, const pm::Expression &exp_y)
    : QObject()
    , m_exp_x(pm::Expression(exp_x.c_str()))
    , m_exp_y(exp_y)
{

}

PointE::PointE(const pm::Expression &exp_x, const std::string &exp_y)
    : QObject()
    , m_exp_x(exp_x)
    , m_exp_y(pm::Expression(exp_y.c_str()))
{

}

PointE::PointE(const std::string &exp_x, const std::string &exp_y)
    : QObject()
    , m_exp_x(pm::Expression(exp_x.c_str()))
    , m_exp_y(pm::Expression(exp_y.c_str()))
{

}

PointE::PointE()
    : QObject()
    , m_exp_x(pm::Expression("0"))
    , m_exp_y(pm::Expression("0"))
{

}


PointE::PointE(const PointE &point)
    : QObject()
    , m_exp_x(point.m_exp_x)
    , m_exp_y(point.m_exp_y)
{

}

PointE::~PointE()
{

}

PointE &PointE::operator=(const PointE &point)
{
    m_exp_x = point.m_exp_x;
    m_exp_y = point.m_exp_y;
    return *this;
}

Expression PointE::distance(const PointE &point)
{
    pm::Expression d_x = x() - point.m_exp_x;
    pm::Expression d_y = y() - point.m_exp_y;
    return (d_x * d_x + d_y * d_y).sqrt();
}

PointE PointE::operator-(const PointE &point)
{
    pm::Expression d_x = x() - point.m_exp_x;
    pm::Expression d_y = y() - point.m_exp_y;
    return PointE(d_x, d_y);
}

pm::Expression &PointE::x()
{
    return m_exp_x;
}

pm::Expression &PointE::y()
{
    return m_exp_y;
}

void PointE::set_trans(const TransformE &trans)
{
    Expression m_x_old = m_exp_x;
    Expression m_y_old = m_exp_y;
    m_exp_x = m_x_old * trans.m11() + m_y_old * trans.m21() + trans.dx();
    m_exp_y = m_y_old * trans.m22() + m_x_old * trans.m12() + trans.dy();
}

void PointE::translate(const pm::Expression &x, const pm::Expression &y)
{
    m_exp_x = m_exp_x + x;
    m_exp_y = m_exp_y + y;
}

}
