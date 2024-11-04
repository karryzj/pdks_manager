#ifndef PARAMUTILS_H
#define PARAMUTILS_H

#include "pmCommon.h"
#include "qvector.h"
#include <QPointF>

#include "systemSundialsLink.h"

namespace GiNaC
{
class ex;
}


namespace pm
{
class ParamDecl;
class Expression;
class ParamMgr;

class PM_PUBLIC ParamUtils final
{

public:
    // params expand
    static QVector<ParamDecl> expand(const QVector<ParamDecl> &params);

    // expression collect
    static void collect(Expression &exp);

    static std::string str(GiNaC::ex ex);

    static QString str_to_3_pre_str(const QString& str);

    static void modify_paramDecl(ParamDecl& param, qreal value);

    static void modify_paramDecl(ParamDecl& param, const QString& exp, ParamMgr* mgr);

    static QVector<QPointF> gen_curve_points(ParamMgr *pm_mgr, const QVector<ParamDecl> &params, double min_dist_threshold);

private:
    static void set_x_t_range(const QString &func, QuanSystem::Interval &x, QuanSystem::Interval &t, bool &has_t);

};

}

#endif // PARAMUTILS_H
