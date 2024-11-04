#include "common_defines.h"
#include "paramDecl.h"
#include "expression.h"
#include "paramUtils.h"
#include "expressionUtil.h"
#include "muParser.h"
#include "ParamMgr.h"

#include <QDebug>

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

void ParamUtils::modify_paramDecl(ParamDecl &param, qreal value)
{
    QString exp = QString::number(value);
    param.set_value(value);
    param.set_expression(exp);
}

void ParamUtils::modify_paramDecl(ParamDecl &param, const QString &exp, ParamMgr *mgr)
{
    qreal value = pm::Expression(exp).to_double(mgr);
    param.set_value(value);
    param.set_expression(exp);
}

QVector<QPointF> ParamUtils::gen_curve_points(ParamMgr *pm_mgr, const QVector<ParamDecl> &params, double min_dist_threshold)
{

    mu::Parser *mu_parser = pm_mgr->mp_parser;

    //加参
    double _x, _y, _t;
    mu_parser->DefineVar(L"x", &_x);
    mu_parser->DefineVar(L"y", &_y);
    mu_parser->DefineVar(L"t", &_t);

    QVector<QPointF> q_points;

    try
    {
        QuanSystem::LocalArea area(QuanSystem::Interval(-100.0, 100.0), QuanSystem::Interval(-100.0, 100.0));

        QuanSystem::SystemSundias cSys(mu_parser);
        cSys.getMinDistThreshold() = min_dist_threshold;

        QuanSystem::Interval x(0, 0);
        QuanSystem::Interval t(0, 0);
        bool has_t = false;

        for(ParamDecl param : params)
        {
            if(param.key().isEmpty())
            {
                cSys.addFuncExpression(param.expression().toStdWString());

                set_x_t_range(param.expression(), x, t, has_t);
            }
        }

        mu::varmap_type variables = mu_parser->GetVar();
        QuanSystem::RPointList points = has_t ? cSys.sample2DPoint(area, variables[L"x"], variables[L"y"], t, variables[L"t"])
                                        : cSys.sample2DPoint(area, variables[L"x"], variables[L"y"], x);


        for(QuanSystem::RPoint p : points)
        {
            q_points.push_back(QPointF(p.x, p.y));
        }
    }
    catch(...)
    {
        //qDebug() << "gen_curve_points: " << ex.what();
    }

    //去参（恢复）
    mu_parser->RemoveVar(L"x");
    mu_parser->RemoveVar(L"y");
    mu_parser->RemoveVar(L"t");

    return q_points;

}

void ParamUtils::set_x_t_range(const QString &func, QuanSystem::Interval &x, QuanSystem::Interval &t, bool &has_t)
{
    int cmp_symbol = 0;
    bool is_t = false, is_x = false;
    QString num_str;

    for(int i = 0; i < func.length(); i++)
    {
        QChar c  = func.at(i);
        if(c == ' ')
        {
            continue;
        }
        else if(c == 'x')
        {
            is_x = 1;
        }
        else if(c == 't')
        {
            is_t = 1;
        }
        else if(c == '>')
        {
            if(func.at(i + 1) == '=')i++;

            cmp_symbol = 1;
        }
        else if(c == '<')
        {
            if(func.at(i + 1) == '=')i++;

            cmp_symbol = -1;
        }
        else if(c == '.' || c == '-' || (c.isNumber())) //数字
        {
            num_str.push_back(c);
        }
        else
        {
            return;
        }
    }


    if(is_x)
    {
        double num = num_str.toDouble();
        cmp_symbol > 0 ? x.start = num : x.end = num;
    }

    if(is_t)
    {
        has_t = true;

        double num = num_str.toDouble();
        cmp_symbol > 0 ? t.start = num : t.end = num;
    }


}

}

