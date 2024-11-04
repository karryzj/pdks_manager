#include "shapeUtils.h"
#include "common_defines.h"
#include "shapeDefines.h"
#include <ParamDecl.h>
#include <ParamMgr.h>
#include <qregularexpression.h>
#include <qstack.h>
#include "paramUtils.h"

using namespace sp;

bool ShapeUtils::is_equal(qreal q_1, qreal q_2)
{
    return abs(q_1 - q_2) > SHAPE_COORD_PRECISION ? false : true;
}

bool sp::ShapeUtils::is_equal(QPointF point_1, QPointF point_2)
{
    if(
        ShapeUtils::is_equal(point_1.x(), point_2.x()) &&
        ShapeUtils::is_equal(point_1.y(), point_2.y()))
    {
        return true;
    }

    return false;
}

QVector<QString> ShapeUtils::split_string(const QString &text, const QString& delimiter)
{
    QVector<QString> lines;
    QStringList parts = text.split(delimiter);
    for(auto str : parts)
    {
        lines.append(str);
    }
    return lines;
}

bool ShapeUtils::is_point_coord(const QString &str, pm::ParamMgr* param_mgr)
{
    QVector<QString> split_string = ShapeUtils::split_string(str, ",");
    if(split_string.size() != 2)
    {
        return false;
    }

    for(const auto& sub_str : split_string)
    {
        if(sub_str.isEmpty())
        {
            return false;
        }


        bool is_num = pm::Expression::is_num(sub_str);
        bool is_exp = pm::Expression::isExpression(param_mgr, sub_str);
        if(false == is_num && false == is_exp)
        {
            return false;
        }
    }

    return true;
}
