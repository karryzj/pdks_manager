#include "shapeDrawBooleanProcessor.h"
#include "dbEdgeProcessor.h"

#define MAGNITFY_RATE 1000.0

using namespace sp;

QPainterPath ShapeDrawBooleanProcessor::boolean_substraction_opera(const QPainterPath &add_path, const QVector<QPainterPath> &delete_paths)
{
    if(delete_paths.isEmpty())
    {
        return add_path;
    }

    std::vector<db::Polygon> a = {};
    a.push_back(ShapeDrawBooleanProcessor::path_to_db_polygon(add_path));

    std::vector<db::Polygon> b = {};
    for(const auto& delete_path : delete_paths)
    {
        b.push_back(ShapeDrawBooleanProcessor::path_to_db_polygon(delete_path));
    }

    std::vector<db::Polygon> out;
    db::EdgeProcessor edge_processor;
    edge_processor.boolean(a, b, out, db::BooleanOp::ANotB);
    if(out.size() == 0)
    {
        return QPainterPath{};
    }

    QPainterPath result;
    for(const auto& out_polygen : out)
    {
        result.addPolygon(ShapeDrawBooleanProcessor::db_polygen_to_qt_polygen(out_polygen));
    }
    result.closeSubpath();
    return result;
}

db::Polygon ShapeDrawBooleanProcessor::path_to_db_polygon(const QPainterPath &path)
{
    QPolygonF polygon = path.toFillPolygon();

    // HINT@leixunyong。这里有从double到int的精度损失
    std::vector<db::point<int>> points;
    for(auto itor = polygon.begin(); itor != polygon.end(); itor++)
    {
        auto magnify_x = itor->x() * MAGNITFY_RATE;
        auto magnify_y = itor->y() * MAGNITFY_RATE;
        db::point<int> pt(magnify_x, magnify_y);  // HINT@leixunyong。这里
        points.push_back(pt);
    }

    db::Polygon poly;
    poly.assign_hull(points.begin(), points.end());
    return poly;
}

QPolygonF ShapeDrawBooleanProcessor::db_polygen_to_qt_polygen(const db::Polygon &db_polygen)
{
    QPolygonF qt_polygen;
    for (auto it = db_polygen.begin_hull(); it != db_polygen.end_hull(); ++it)
    {
        auto x = (*it).x();
        auto y = (*it).y();
        qt_polygen << QPointF(x / MAGNITFY_RATE, y / MAGNITFY_RATE);
    }
    return qt_polygen;
}
