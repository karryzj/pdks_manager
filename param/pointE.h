#ifndef POINTE_H
#define POINTE_H
#include <QObject>
#include <QTransform>

#include "expression.h"
#include "pmCommon.h"


namespace pm
{

class TransformE;

class PM_PUBLIC PointE final : public QObject
{
    Q_OBJECT
public:
    PointE(const pm::Expression &exp_x, const pm::Expression &exp_y);
    PointE(const std::string &exp_x, const pm::Expression &exp_y);
    PointE(const pm::Expression &exp_x, const std::string &exp_y);
    PointE(const std::string &exp_x, const std::string &exp_y);
    PointE();
    PointE(const PointE &point);
    ~PointE();

    PointE &operator=(const PointE &point);

    //for calc 2 point distance.
    Expression distance(const PointE &point);

    PointE operator-(const PointE &point);

    pm::Expression &x();
    pm::Expression &y();

    //transform
    void set_trans(const TransformE &trans);
    void translate(const pm::Expression &x, const pm::Expression &y);

private:
    pm::Expression m_exp_x;
    pm::Expression m_exp_y;
};

}


#endif // POINTE_H
