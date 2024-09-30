#ifndef SHAPEDRAWBOOLEANPROCESSOR_H
#define SHAPEDRAWBOOLEANPROCESSOR_H

#include "dbPolygon.h"
#include "qpainterpath.h"
#include "spCommon.h"

namespace sp
{
class SP_PUBLIC ShapeDrawBooleanProcessor final
{
public:
    ShapeDrawBooleanProcessor() = delete;
    ~ShapeDrawBooleanProcessor() = delete;

    static QPainterPath boolean_substraction_opera(const QPainterPath& add_path, const QVector<QPainterPath>&delete_paths);
private:
    static db::Polygon path_to_db_polygon(const QPainterPath& path);
    static QPolygonF db_polygen_to_qt_polygen(const db::Polygon& db_polygen);
};
}



#endif // SHAPEDRAWBOOLEANPROCESSOR_H
