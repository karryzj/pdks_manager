#ifndef SHAPEBASE_H
#define SHAPEBASE_H

#include <QObject>
#include <QPointF>
#include <QVector>
#include "paramDecl.h"
#include <shapedrawgraphicsitem.h>
#include "spCommon.h"

QT_BEGIN_NAMESPACE
class QGraphicsItem;
class QPointF;
class QPainterPath;
QT_END_NAMESPACE

namespace pm
{
class ParamMgr;
class PointE;
}
namespace sp
{

class ShapeFactory;
class SP_PUBLIC ShapeBase : public QObject
{
    Q_OBJECT
    friend class ShapeFactory;
protected:
    ShapeBase(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl>& params, ShapePointGraphicsItem* parent_attach_point);

public:
    ~ShapeBase() override;

    //param part
    void set_new_param_mgr(pm::ParamMgr *param_mgr);  // 一般来讲不会调用此接口。
    void set_new_params(const QVector<pm::ParamDecl>& params);
    const QVector<pm::ParamDecl> &params();
    bool param_used(const QString &key);
    pm::ParamMgr *param_mgr() const;
    double rotate() const;
    pm::Expression rotate_exp() const;
    void update();

    const QString &shape_type();
    QString shape_name() const;

    const QVector<QPointF>& attach_points();
    const QVector<pm::PointE>& attach_exp_points();
    QPainterPath& painter_path();  // HINT@leixunyong。这里需要进行变换，所以引用出去。

    //graphic
    ShapeDrawGraphicsItem *shape_graphics_item() const;

    const QVector<ShapePointGraphicsItem*>& point_graphics_items() const;

    void replace_graphics_items(ShapeBase* other_shape);

    void set_anchor_points(const QVector<QPointF>& points);
    QVector<QPointF> get_anchor_points();

    void set_coord_anchor_point(const QPointF &point);
    QPointF get_coord_anchor_point();

    void set_parent_attach_point(sp::ShapePointGraphicsItem* point_item);
    sp::ShapePointGraphicsItem* parent_attach_point() const;

private:
    void set_point_graphics_items(const QVector<ShapePointGraphicsItem*>& point_items);
    void remove_point_graphics_items(); // HINT@leixunyong。对应内存需要托管给另外一个ShapeBase才能调用此接口。

    void set_shape_graphics_item(ShapeDrawGraphicsItem* shape_item);
    void remove_shape_item(); // HINT@leixunyong。对应内存需要托管给另外一个ShapeBase才能调用此接口。
protected:
    virtual void update_variables();
    virtual void update_attach_pointes() = 0;
    virtual QPainterPath build_path() = 0;

private:
    void update_attach_points();
    void update_path();

protected:
    double m_rotate;
    std::string m_rotate_str;

    QVector<pm::ParamDecl> m_params;
    pm::ParamMgr *mp_param_mgr = nullptr;
    QVector<QPointF> m_attach_points;
    QVector<pm::PointE> m_attach_points_e;
    QPainterPath m_path;

    QVector<QPointF> m_anchor_points;
    QPointF m_coord_anchor_point;

    ShapeDrawGraphicsItem * mp_shape_item = nullptr;
    QVector<ShapePointGraphicsItem*> m_point_items;

private:
    int m_id;
    QString m_shape_name;

    ShapePointGraphicsItem* mp_parent_attach_point;
};

}
#endif // SHAPEBASE_H
