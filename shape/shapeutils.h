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

    static QVector<QString> split_string(const QString& text, const QString& delimiter);

    static bool is_point_coord(const QString& str, pm::ParamMgr* param_mgr); // 检查输入的字符串是形如 1,1 这样的坐标表示。
};
}

#endif // SHAPEUTILS_H
