#ifndef SHAPECURVE_H
#define SHAPECURVE_H

#include <ShapeBase.h>
#include <ShapeFactoryBase.h>

namespace sp
{

// shape param setting and produce graphics
class SP_PUBLIC ShapeCurve final: public ShapeBase
{
public:
    ShapeCurve(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl>& params, ShapePointGraphicsItem* parent_attach_point);
    ~ShapeCurve() override;

public:
    const QVector<QPointF>& point_set() const;
    void update_variables() override;//调整为public给c++ 的pcell使用

protected:
    void update_attach_pointes() override;
    QPainterPath build_path() override;

private:
    void analyse_equation(const QString& equation, QVector<QPointF>& point_set, bool reverse);
    void analyse_fixed_point_set(const QString& fixed_point_set, QVector<QPointF>& point_set, bool reverse);

private:
    QVector<QPointF> m_point_set;
};

class ShapeCurveFactory final : public ShapeFactoryBase
{
public:
    ShapeCurveFactory();
    ~ShapeCurveFactory() override;

    ShapeBase *create_shape(const QString &shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const override;
};

}

#endif // SHAPECURVE_H
