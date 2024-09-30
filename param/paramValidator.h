#ifndef PARAMVALIDATOR_H
#define PARAMVALIDATOR_H

#include <QValidator>
#include "pmCommon.h"

namespace pm
{
class PM_PUBLIC ParamValidator final: public QValidator
{
public:
    explicit ParamValidator(QObject *parent);
    ~ParamValidator() override;

    State validate(QString &input, int &pos) const override;
};
}

#endif // PARAMVALIDATOR_H
