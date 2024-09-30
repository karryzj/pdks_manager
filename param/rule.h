#ifndef RULE_H
#define RULE_H

#include <QString>
#include <QObject>

#include "expression.h"
#include "pmCommon.h"

namespace pm
{

class ParamMgr;

// one param rule ie: x1 > y1
class PM_PUBLIC Rule : public Expression
{
public:
    explicit Rule(QString rule_str);
    Rule(const Rule &rule);
    ~Rule();

    bool valid(ParamMgr *mg);
};

}

#endif // RULE_H

