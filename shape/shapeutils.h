#ifndef SHAPEUTILS_H
#define SHAPEUTILS_H

#include <QPointF>
#include <qstring.h>
#include "spCommon.h"

class QDialog;
class QWidget;
class QVBoxLayout;

namespace pm
{
class ParamDecl;
class ParamMgr;
}

namespace sp
{
class SP_PUBLIC ShapeUtils final
{
public:
    ShapeUtils() = delete;
    // HINT：这里用来添加一些公用的接口，或者封装一些固定逻辑。
    static bool is_equal(qreal q_1, qreal q_2);
    static bool is_equal(QPointF point_1, QPointF point_2);
};
}

#endif // SHAPEUTILS_H
