#ifndef TRANSFORME_H
#define TRANSFORME_H

#include "expression.h"

namespace pm
{

class PM_PUBLIC TransformE : public QObject
{
    Q_OBJECT
public:
    TransformE();
    TransformE(const TransformE &trans);
    ~TransformE();

    TransformE &operator =(const TransformE &trans);

    const Expression &m11() const;
    const Expression &m12() const;
    const Expression &m21() const;
    const Expression &m22() const;
    const Expression &dx() const;
    const Expression &dy() const;


    TransformE &scale(const Expression &scale_x, const Expression &scale_y);
    // no use
    TransformE &translate(const Expression &dx, const Expression &dy);
    // rotate to
    TransformE &rotate(const Expression &degree);


private:
    Expression m_11;
    Expression m_12;
    Expression m_21;
    Expression m_22;
    Expression m_dx;
    Expression m_dy;

    // or use below (not supported yet)
    //Expression m_13;
    //Expression m_23;
    //Expression m_33;

};

}

#endif // TRANSFORME_H
