#include <QRegularExpression>
#include <math.h>
#include "expression.h"
#include "paramMgr.h"
#include "muParser.h"
#include "paramDecl.h"
#include "paramUtils.h"
#include "numUtils.h"

namespace pm
{

Expression::Expression(const QString &str)
    :  m_str(str)
{
    if (!str.isEmpty() && !is_num() && !is_bool(m_str))
    {
        // HINT@leixunyong。这里经常会出现闪退，且不报任何信息。
        ParamUtils::collect(*this);
    }

}

Expression::Expression(double val)
    : Expression(std::to_string(val).c_str())
{

}

Expression::Expression(const Expression &expression)
    : m_str(expression.m_str)
{
    if (!m_str.isEmpty() && !is_num() &&  !is_bool(m_str))
        ParamUtils::collect(*this);
}

Expression::~Expression()
{
}

Expression &Expression::operator=(const Expression &expression)
{
    m_str = expression.m_str;
    return *this;
}

float Expression::to_float(ParamMgr *mgr)
{
    if(is_num())
        return m_str.toFloat();

    float ret = mgr->parse(m_str);
    return ret;
}

int Expression::to_int(ParamMgr *mgr)
{
    return std::round(to_float(mgr));
}

double Expression::to_double(ParamMgr *mgr) const
{
    if(is_num())
        return m_str.toDouble();

    double ret = mgr->parse(m_str);
    return ret;
}

QString Expression::to_str() const
{
    return m_str;
}

bool Expression::is_zero(int precision) const
{
    if(m_str == "0")
        return true;

    double val = to_double();
    return cm::NumUtils::is_zero(val);
}

Expression  Expression::operator -() const
{
    if(is_exp())
    {
        return is_positive_variable() ? "-" + m_str : "-(" + m_str + ")";
    }
    else if(is_zero())
    {
        return m_str;
    }
    else
    {
        // neg num check
        return is_begin_neg() ? m_str.mid(1) : "-" + m_str;
    }
}

Expression Expression::operator *(double f) const
{
    QString ret_str;
    if(cm::NumUtils::equal(f, 0))
        return QString("0");

    if(cm::NumUtils::equal(f, 1))
        return m_str;

    if(!is_exp())
    {
        ret_str = std::to_string(std::stod(m_str.toStdString()) * f).c_str();
    }
    else
    {
        ret_str = QString("(%1)*%2").arg(m_str).arg(f);
    }
    return ret_str;
}

Expression Expression::operator *(const Expression & exp) const
{
    if(exp.is_num())
    {
        return *this * exp.to_double();
    }
    else if(this->is_num())
    {
        return exp * this->to_double();
    }

    //two expression
    return QString("(%1)*(%2)").arg(m_str).arg(exp.m_str);
}

Expression Expression::sqrt() const
{
    if(is_num())
    {
        return QString::number(std::sqrt(to_double()));
    }

    return QString("sqrt(%1)").arg(m_str);
}

Expression Expression::sin() const
{
    if(is_num())
    {
        return QString::number(std::sin(to_double()));
    }

    return QString("sin(%1)").arg(m_str);
}

Expression Expression::cos() const
{
    if(is_num())
    {
        return QString::number(std::cos(to_double()));
    }

    return QString("cos(%1)").arg(m_str);
}

Expression Expression::operator +(Expression exp) const
{
    bool is_exp = this->is_exp();
    bool is_exp_other = exp.is_exp();
    if(!is_exp && !is_exp_other)
    {
        float result = QVariant(m_str).toFloat() +  QVariant(exp.m_str).toFloat();
        return Expression(std::to_string(result).c_str());
    }
    else if(!is_exp && is_zero())
    {
        return exp.m_str;
    }
    else if(!is_exp_other && exp.is_zero())
    {
        return m_str;
    }
    else
    {
        return m_str + (exp.is_begin_neg() ? exp.m_str : "+" + exp.m_str);
    }
}

Expression Expression::operator +(double add) const
{
    if(cm::NumUtils::equal(add, 0))
        return m_str;

    QString ret_str;
    if(!is_exp())
    {
        ret_str = std::to_string(std::stod(m_str.toStdString()) + add).c_str();
    }
    else
    {
        ret_str = QString("%1+%2").arg(m_str).arg(add);
    }
    return ret_str;
}

Expression Expression::operator -(Expression exp) const
{
    return *this + (-exp);
}

bool Expression::operator ==(Expression &expression)
{
    return m_str == expression.m_str;
}

bool Expression::isExpression(ParamMgr *mgr) const
{
    auto ret = mgr->parse(m_str);
    if (ret == 0)
        return false;
    return true;
}

bool Expression::isExpression(ParamMgr *mgr, const QString str)
{
    if (is_bool(str))
    {
        auto ret = mgr->parse(str);
        if (ret == 0)
            return false;
        return true;
    }
    else
    {
        bool ok = true;
        auto ret = mgr->calc_expr(str, ok);
        if (ok)
            return true;
        else
            return false;
    }
}

bool Expression::is_expression(QString str)
{
    return !Expression::is_num(str);
}

bool Expression::is_num(QString str)
{
    // 普通数字
    QRegExp num_reg = QRegExp("^[-]{0,1}[0-9.]+$");
    return num_reg.exactMatch(str);
}

bool Expression::is_begin_neg(QString str)
{
    return str.length() > 0 && str.at(0) == '-';
}

bool Expression::is_positive_variable(QString str)
{
    QRegExp negtive_num_reg = QRegExp("^[a-zA-z]+[a-zA-z0-9_]*$");
    return negtive_num_reg.exactMatch(str);
}

bool Expression::is_exp() const
{
    return Expression::is_expression(m_str);
}

bool Expression::is_num() const
{
    return Expression::is_num(m_str);
}

bool Expression::is_begin_neg() const
{
    return Expression::is_begin_neg(m_str);
}

bool Expression::is_positive_variable() const
{
    return Expression::is_positive_variable(m_str);
}

bool Expression::is_bool(const QString& expression)
{
    // 定义一个正则表达式，用于匹配关系运算符
    QRegularExpression regex(R"(==|!=|<=|>=|<|>)");
    QRegularExpressionMatch match = regex.match(expression);

    // 如果匹配成功，返回 true，否则返回 false
    return match.hasMatch();
}

}

