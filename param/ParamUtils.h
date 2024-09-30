#ifndef PARAMUTILS_H
#define PARAMUTILS_H

#include "pmCommon.h"
#include "qvector.h"

namespace GiNaC
{
class ex;
}

namespace pm
{
class ParamDecl;
class Expression;

class PM_PUBLIC ParamUtils final
{

public:
    // params expand
    static QVector<ParamDecl> expand(const QVector<ParamDecl> &params);

    // expression collect
    static void collect(Expression &exp);

    static std::string str(GiNaC::ex ex);

    static QString str_to_3_pre_str(const QString& str);
};

}

#endif // PARAMUTILS_H
