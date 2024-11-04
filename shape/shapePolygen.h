#ifndef SHAPEPOLYGEN_H
#define SHAPEPOLYGEN_H

#include <ShapeBase.h>
#include <ShapeFactoryBase.h>

namespace sp
{
class ShapePolygen final : public ShapeBase
{
public:
    ShapePolygen(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> & params, ShapePointGraphicsItem* parent_attach_point);
    ~ShapePolygen() override;

protected:
    void update_variables() override;
    void update_attach_pointes() override;
    QPainterPath build_path() override;

private:
    void remove_no_coord_params(QVector<pm::ParamDecl> & params);

private:
    mutable QVector<QPointF> m_points;
};

class ShapePolygenFactory final : public ShapeFactoryBase
{
public:
    ShapePolygenFactory();
    ~ShapePolygenFactory() override;

    ShapeBase *create_shape(const QString& shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const override;
};

}



#endif // SHAPEPOLYGEN_H
