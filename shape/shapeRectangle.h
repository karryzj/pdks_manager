#ifndef SHAPERECTANGLE_H
#define SHAPERECTANGLE_H

#include "shapeBase.h"
#include "shapeFactoryBase.h"

namespace sp
{
// shape param setting and produce graphics
class SP_PUBLIC ShapeRectangle final: public ShapeBase
{
public:
    ShapeRectangle(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> & params, ShapePointGraphicsItem* parent_attach_point);
    ~ShapeRectangle() override;

protected:
    void update_variables() override;
    void update_attach_pointes() override;
    QPainterPath build_path() override;


private:
    mutable qreal m_width;
    mutable qreal m_height;

    mutable std::string m_width_exp;
    mutable std::string m_height_exp;
};


// rec shape factory only create RectShape
class ShapeRectangleFacotry final : public ShapeFactoryBase
{
public:
    ShapeRectangleFacotry();
    ~ShapeRectangleFacotry() override;

    ShapeBase *create_shape(const QString &shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const override;
};

}

#endif // SHAPERECTANGLE_H
