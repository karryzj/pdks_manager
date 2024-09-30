#ifndef PARAMDECL_H
#define PARAMDECL_H



#include <QString>
#include <QObject>
#include <QVariant>
#include "pmCommon.h"
namespace pm
{

class ParamMgr;

//one param declaration ie: x1 = 50
class PM_PUBLIC ParamDecl
{

public:
    ParamDecl();
    explicit ParamDecl(QString key, QString expression, QString desc = "");
    explicit ParamDecl(QString key, double value, QString desc = "");
    explicit ParamDecl(QString key, QVariant val, QString desc = "");
    ~ParamDecl();

    // 拷贝构造函数
    ParamDecl(const ParamDecl& other);

    // 赋值运算符重载
    ParamDecl& operator=(const ParamDecl& other);

    inline bool operator==(const ParamDecl &rhs) const
    {
        return m_key == rhs.m_key &&
               m_expression == rhs.m_expression &&
               m_desc == rhs.m_desc;
    }

    inline bool operator!=(const ParamDecl &rhs)
    {
        return !(*this == rhs);
    }

    //info
    const QString &key() const;
    const QString &expression() const;
    const QString &desc() const;
    double value() const;
    // type conversion
    float to_float(ParamMgr *mgr);    
    double to_double(ParamMgr *mgr) const;
    int to_int(ParamMgr *mgr);
    QString to_str() const;

    //if expression
    bool is_expression(ParamMgr *mgr) const;

    //set new value
    void reset_value(const QVariant &value);
    void set_key(const QString &key);
    void set_desc(const QString &desc);
    void set_value(double value);
    double real_value() const;
    void set_expression(const QString &expression);

    // gsi use
    double to_double() const{return m_value;};
    int to_int() const{return int(m_value);};

private:
    QString m_key = "";
    QString m_expression = "";
    double m_value = 0;
    QString m_desc = "";


};

}


#endif // PARAMDECL_H
