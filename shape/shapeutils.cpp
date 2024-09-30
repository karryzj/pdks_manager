#include "shapeUtils.h"
#include "common_defines.h"
#include "shapeDefines.h"
#include <ParamDecl.h>
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
