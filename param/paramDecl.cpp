#include <QVariant>
#include <cmath>
#include "paramMgr.h"
#include "paramDecl.h"
#include "expression.h"
#include "rule.h"

namespace pm
{

ParamDecl::ParamDecl()
{

}

ParamDecl::ParamDecl(QString key, QString expression, QString desc)
    : m_key(key)
    , m_expression(expression)
    , m_desc(desc)
{
    bool ret_ok = false;
    double ret_val = expression.toDouble(&ret_ok);
    if (ret_ok)
    {
        m_value = ret_val;
    }
}

ParamDecl::ParamDecl(QString key, double value, QString desc)
    : m_key(key), m_value(value), m_expression(QString::number(value)), m_desc(desc)
{

}

ParamDecl::ParamDecl(QString key, QVariant val, QString desc)
    : m_key(key), m_desc(desc)
{
    if (val.type() == QVariant::Double)
    {
        double new_val = val.toDouble();
        *this = ParamDecl(key, new_val, desc);
    }
    else
    {
        QString new_val = val.toString();
        *this = ParamDecl(key, new_val, desc);
    }
}

ParamDecl::ParamDecl(const ParamDecl& other)
{
    m_key = other.m_key;
    m_expression = other.m_expression;
    m_value = other.m_value;
    m_desc = other.m_desc;
}

ParamDecl& ParamDecl::operator=(const ParamDecl& other)
{
    if (this == &other) return *this; // 防止自我赋值
    m_key = other.m_key;
    m_expression = other.m_expression;
    m_value = other.m_value;
    m_desc = other.m_desc;

    return *this;
}

ParamDecl::~ParamDecl()
{
}

const QString &ParamDecl::key() const
{
    return m_key;
}

const QString &ParamDecl::desc() const
{
    return m_desc;
}

float ParamDecl::to_float(ParamMgr *mgr)
{
    if(!is_expression(mgr))
    {
        return m_value;
    }
    else
    {
        return Expression(m_expression).to_float(mgr);
    }
}

double ParamDecl::to_double(ParamMgr *mgr) const
{
    if(!is_expression(mgr))
    {
        return m_value;
    }
    else
    {
        return Expression(m_expression).to_double(mgr);
    }
}

int ParamDecl::to_int(ParamMgr *mgr)
{
    if(!is_expression(mgr))
    {
        return std::round(m_value);
    }
    else
    {
        return Expression(m_expression).to_int(mgr);
    }
}

QString ParamDecl::to_str() const
{
    return m_expression;
}

bool ParamDecl::is_expression(ParamMgr *mgr) const
{
    // 是否带有特殊字符
    bool is_expression =  Expression::is_expression(to_str());
    if(!is_expression)
    {
        return is_expression;
    }

    // 判断  参数变量是否都包含，确认是合法表达式 ; 是否可以计算
    // todo
    return true;
}

void ParamDecl::reset_value(const QVariant &value)
{
    if(m_value == value)
    {
        return;
    }

}

void ParamDecl::set_key(const QString &key)
{
    m_key = key;
}

void ParamDecl::set_desc(const QString &desc)
{
    m_desc = desc;
}

void ParamDecl::set_value(double value)
{
    // m_expression = QString::number(value);
    m_value = value;
}

const QString& ParamDecl::expression() const
{
    return m_expression;
}

void ParamDecl::set_expression(const QString &expression)
{
    m_expression = expression;
}

double ParamDecl::value() const
{
    return m_value;
}



}
