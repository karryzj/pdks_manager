#ifndef PARAMMGR_H
#define PARAMMGR_H


#include <QMap>
#include <QVector>
#include <QString>
#include <QObject>
#include "pmCommon.h"
#include "rule.h"
#include "paramDecl.h"
#include "pmCommon.h"

namespace mu
{
class Parser;
}

namespace pm
{


class ParamDecl;
class Rule;


// global params for one a attach tree.
class PM_PUBLIC ParamMgr final : public QObject
{
    Q_OBJECT
public:
    friend class ParamUtils;
    ParamMgr();
    ~ParamMgr() override;

    // basic param operations.
    void add_param(ParamDecl *param);
    void del_param(ParamDecl *param);
    void del_param(const QString &key);
    void update_param(ParamDecl *param);
    QList<ParamDecl*> clone_params();
    ParamDecl* clone_param(ParamDecl *param);
    QList<ParamDecl*>& params();
    ParamDecl * find_param(const QString& param_name);
    void refresh_params_value();

    // basic rule operations
    bool add_rule(const Rule &rule);
    void del_rule(const Rule &rule);
    void del_rule(const QString &rule_str);
    QVector<Rule>& rules();
    bool isValid();
    bool isValid(const Rule &rule);
    bool isValid(ParamDecl *param);
    bool isValid(const ParamDecl *param, const Rule &rule);
    bool parse_rule(const Rule &rule);
    double parse(const QString &exp);
    double calc_expr(const QString &exp);
    double calc_expr(const QString &exp, bool &ok);
    void reset();
    QVector<Rule> clone_rules();

private:
    QList<ParamDecl*> m_params;
    QVector<Rule> m_rules;
    mu::Parser *mp_parser;

signals:
    void param_changed(const QString &key);


};

}

#endif // PARAMMGR_H
