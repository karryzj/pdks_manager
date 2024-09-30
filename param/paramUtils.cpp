#include "common_defines.h"
#include "paramDecl.h"
#include "expression.h"
#include "paramUtils.h"
#include "expressionUtil.h"

namespace pm
{
QVector<ParamDecl> ParamUtils::expand(const QVector<ParamDecl> &params)
{
    int len = params.size();
    cc_param *ps = (cc_param*)malloc(len * sizeof(cc_param));

    for(int i = 0; i < len; i++)
    {
        ps[i].key = strdup(params.at(i).key().toStdString().c_str());
        ps[i].val = strdup(params.at(i).expression().toStdString().c_str());
    }

    cc_param *ret_ps = cc_expand(ps, len);

    //free input
    for(int i = 0; i < len; i++)
    {
        free(ps[i].key);
        free(ps[i].val);
    }
    free(ps);
    //free input end

    QVector<ParamDecl> params_ret;

    for(int i = 0; i < len; i++)
    {
        QString key = ret_ps[i].key;
        QString expression = ret_ps[i].val;
        params_ret.push_back(ParamDecl(key, expression));

        cc_free(ret_ps[i].key);
        cc_free(ret_ps[i].val);
    }
    cc_free(ret_ps);

    return params_ret;
}

void ParamUtils::collect(Expression &exp)
{
    QString str = exp.m_str;
    char* ret_str = cc_collect(str.toStdString().c_str());
    exp.m_str = ret_str;
    cc_free(ret_str);
}

QString ParamUtils::str_to_3_pre_str(const QString &str)
{
    bool ok;
    double num = str.toDouble(&ok);
    if(ok)
    {
        return QString::number(TO_THREE_DECIMALS(num));
    }
    return str;
}
}
