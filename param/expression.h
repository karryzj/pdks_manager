#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <QString>
#include <QObject>
#include "pmCommon.h"

namespace mu
{
class Parser;
}
namespace pm
{

class ParamMgr;

// param expression ie:  x1+y2
class PM_PUBLIC Expression
{
public:
    friend class ParamUtils;
    Expression(const QString &str);
    Expression(double val);
    Expression(const Expression &expression);
    ~Expression();

    Expression &operator=(const Expression &expression);

    float to_float(ParamMgr *mgr);
    int to_int(ParamMgr *mgr);
    double to_double(ParamMgr *mgr = nullptr) const;
    QString to_str() const;

    bool isExpression(ParamMgr *mgr) const;
    static bool isExpression(ParamMgr *mgr, const QString str);
    static bool is_expression(QString str);
    static bool is_num(QString str);
    static bool is_begin_neg(QString str);
    static bool is_positive_variable(QString str);
    static bool is_bool(const QString& expression);
    bool is_exp() const;
    bool is_num() const;
    bool is_zero(int precision = 4) const;
    bool is_begin_neg() const;
    bool is_positive_variable() const;


    bool operator ==(Expression &expression);
    Expression operator -() const;
    Expression operator -(Expression exp) const;
    Expression operator +(Expression exp) const;
    Expression operator +(double add) const;
    Expression operator *(double f) const;
    Expression operator *(const Expression &exp) const;

    Expression sqrt() const;
    Expression sin() const;
    Expression cos() const;
    Expression deg2rad() const;

protected:
    QString m_str;

};

}



#endif // EXPRESSION_H
