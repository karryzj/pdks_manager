#include "paramValidator.h"

using namespace pm;

ParamValidator::ParamValidator(QObject *parent)
    : QValidator{parent}
{}

ParamValidator::~ParamValidator()
{

}

QValidator::State ParamValidator::validate(QString &input, int &pos) const
{
    // 定义正则表达式，匹配小数点后最多3位的小数
    QRegularExpression regex(R"(^-?\d+(\.\d{0,3})?$)");
    QRegularExpressionMatch match = regex.match(input);

    // 如果匹配正则表达式，返回 Acceptable
    if (match.hasMatch())
    {
        return Acceptable;
    }

    // 尝试将输入转换为 double
    bool ok;
    input.toDouble(&ok);

    // 如果不能转换为数字，返回 Acceptable
    if (!ok)
    {
        return Acceptable;
    }

    // 否则返回 Invalid
    return Invalid;
}
