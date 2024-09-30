#include "transformE.h"
#include "numUtils.h"

namespace pm
{

TransformE::TransformE()
    : QObject()
    , m_11(1)
    , m_12(0)
    , m_21(0)
    , m_22(1)
    , m_dx(0)
    , m_dy(0)
{

}

TransformE::TransformE(const TransformE &trans)
    : QObject()
    , m_11(trans.m_11)
    , m_12(trans.m_12)
    , m_21(trans.m_21)
    , m_22(trans.m_22)
    , m_dx(trans.m_dx)
    , m_dy(trans.m_dy)
{

}

TransformE::~TransformE()
{

}

TransformE &TransformE::operator =(const TransformE &trans)
{
    if(this == &trans)
        return *this;

    m_11 = trans.m_11;
    m_12 = trans.m_12;
    m_21 = trans.m_21;
    m_22 = trans.m_22;
    m_dx = trans.m_dx;
    m_dy = trans.m_dy;

    return *this;
}

const Expression &TransformE::m11() const
{
    return m_11;
}

const Expression &TransformE::m12() const
{
    return m_12;
}

const Expression &TransformE::m21() const
{
    return m_21;
}

const Expression &TransformE::m22() const
{
    return m_22;
}

const Expression &TransformE::dx() const
{
    return m_dx;
}

const Expression &TransformE::dy() const
{
    return m_dy;
}

TransformE &TransformE::scale(const Expression &scale_x, const Expression &scale_y)
{
    m_11 = m_11 * scale_x;
    m_22 = m_22 * scale_y;

    return *this;
}

TransformE &TransformE::translate(const Expression &dx, const Expression &dy)
{
   // current no need
   // todo
    return *this;
}

TransformE &TransformE::rotate(const Expression &degree)
{
    Expression radian = degree * (PI / 180.0);
    Expression sina = radian.sin();
    Expression cosa = radian.cos();

    Expression tm11 = cosa * m_11;
    Expression tm12 = sina * m_22;
    Expression tm21 = -sina * m_11;
    Expression tm22 = cosa * m_22;

    m_11 = tm11;
    m_12 = tm12;
    m_21 = tm21;
    m_22 = tm22;

    return *this;
}



}
