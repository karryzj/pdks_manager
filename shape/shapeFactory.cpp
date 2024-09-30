#include "shapeFactory.h"
#include "shapeDecl.h"
// shape
#include "shapeRectangle.h"
#include "shapeCircle.h"
#include "shapeEllipse.h"
#include "shapeQuadrangle.h"
#include "shapeSector.h"
#include "shapeTriangle.h"
#include "shapePolygen.h"

#include "shapeDefines.h"

namespace sp
{
ShapeFactory::ShapeFactory()
    : ShapeFactoryBase()
{

    QVector<QPair<QString, QVariant>> param_circle{{tr(SHAPE_CIRCLE_RADIUS), SHAPE_CIRCLE_RADIUS_DEFAULT_VALUE}};
    register_shape_decl(new ShapeDecl(tr(SHAPE_CIRCLE), "circle.png",
                                      param_circle,
                                      new ShapeCircleFacotry()));

    QVector<QPair<QString, QVariant>> param_ellipse
    {
        {tr(SHAPE_ELLIPSE_SEMI_MAJOR_AXIS), SHAPE_ELLIPSE_SEMI_MAJOR_AXIS_DEFAULT_VALUE},
        {tr(SHAPE_ELLIPSE_SEMI_MINOR_AXIS), SHAPE_ELLIPSE_SEMI_MINOR_AXIS_DEFAULT_VALUE},
        {tr(SHAPE_ELLIPSE_START_ANGLE), SHAPE_ELLIPSE_START_ANGLE_DEFAULT_VALUE},
        {tr(SHAPE_ELLIPSE_END_ANGLE), SHAPE_ELLIPSE_END_ANGLE_DEFAULT_VALUE},
        {tr(SHAPE_ROTATE), SHAPE_ROTATE_DEFAULT_VALUE}
    };
    register_shape_decl(new ShapeDecl(tr(SHAPE_ELLIPSE), "ellipse.png",
                                      param_ellipse,
                                      new ShapeEllipseFacotry()));

    // 多边形是变参数类型的图形，因此需要额外特殊处理
    QVector<QPair<QString, QVariant>> param_polygen
    {
        {tr(SHAPE_ROTATE), SHAPE_ROTATE_DEFAULT_VALUE}
    };
    register_shape_decl(new ShapeDecl(tr(SHAPE_POLYGEN), "polygen.png",
                                      param_polygen,
                                      new ShapePolygenFacotry(), true));


    QVector<QPair<QString, QVariant>> param_quarangle
    {
        {tr(SHAPE_QUADRANGLE_WIDTH), SHAPE_QUADRANGLE_WIDTH_DEFAULT_VALUE},
        {tr(SHAPE_QUADRANGLE_HEIGHT), SHAPE_QUADRANGLE_HEIGHT_DEFAULT_VALUE},
        {tr(SHAPE_QUADRANGLE_PARAMETER_1), SHAPE_QUADRANGLE_ONE_DEFAULT_VALUE},
        {tr(SHAPE_QUADRANGLE_PARAMETER_2), SHAPE_QUADRANGLE_TWO_DEFAULT_VALUE},
        {tr(SHAPE_QUADRANGLE_PARAMETER_3), SHAPE_QUADRANGLE_THREE_DEFAULT_VALUE},
        {tr(SHAPE_QUADRANGLE_PARAMETER_4), SHAPE_QUADRANGLE_FOUR_DEFAULT_VALUE},
        {tr(SHAPE_ROTATE), SHAPE_ROTATE_DEFAULT_VALUE}
    };
    register_shape_decl(new ShapeDecl(tr(SHAPE_QUADRANGLE), "quadrangle.png",
                                      param_quarangle,
                                      new ShapeQuadrangleFacotry()));

    QVector<QPair<QString, QVariant>> param_rectangle
    {
        {tr(SHAPE_RECTANGLE_WIDTH), SHAPE_RECTANGLE_WIDTH_DEFAULT_VALUE},
        {tr(SHAPE_RECTANGLE_HEIGHT), SHAPE_RECTANGLE_HEIGHT_DEFAULT_VALUE},
        {tr(SHAPE_ROTATE), SHAPE_ROTATE_DEFAULT_VALUE}
    };
    register_shape_decl(new ShapeDecl(tr(SHAPE_RECTANGLE), "rectangle.png",
                                      param_rectangle,
                                      new ShapeRectangleFacotry()));

    QVector<QPair<QString, QVariant>> param_sector
    {
        {tr(SHAPE_SECTOR_INSIDE_RADIUS), SHAPE_SECTOR_INSIDE_RADIUS_DEFAULT_VALUE},
        {tr(SHAPE_SECTOR_OUTSIDE_RADIUS), SHAPE_SECTOR_OUTSIDE_RADIUS_DEFAULT_VALUE},
        {tr(SHAPE_SECTOR_START_ANGLE), SHAPE_SECTOR_START_ANGLE_DEFAULT_VALUE},
        {tr(SHAPE_SECTOR_END_ANGLE), SHAPE_SECTOR_END_ANGLE_DEFAULT_VALUE},
        {tr(SHAPE_ROTATE), SHAPE_ROTATE_DEFAULT_VALUE}
    };
    register_shape_decl(new ShapeDecl(tr(SHAPE_SECTOR), "sector.png",
                                      param_sector,
                                      new ShapSectoreFacotry()));

    QVector<QPair<QString, QVariant>> param_triangle
    {
        {tr(SHAPE_TRIANGLE_WIDTH), SHAPE_TRIANGLE_WIDTH_DEFAULT_VALUE},
        {tr(SHAPE_TRIANGLE_HEIGHT), SHAPE_TRIANGLE_HEIGHT_DEFAULT_VALUE},
        {tr(SHAPE_ROTATE), SHAPE_ROTATE_DEFAULT_VALUE}
    };
    register_shape_decl(new ShapeDecl(tr(SHAPE_TRIANGLE), "triangle.png",
                                      param_triangle,
                                      new ShapeTriangleFacotry()));
}

ShapeFactory::~ShapeFactory()
{
    for(auto i = m_shapes_decl.begin(); i != m_shapes_decl.end(); ++i)
    {
        delete i.value();
    }
    m_shapes_decl.clear();
}

ShapeFactory *ShapeFactory::instance()
{
    static ShapeFactory _instance;
    return &_instance;
}

void ShapeFactory::register_shape_decl(ShapeDecl *decl)
{
    m_shapes_decl.insert(decl->name(), decl);
}

QMap<QString, ShapeDecl*> ShapeFactory::get_shapes_decl() const
{
    return m_shapes_decl;
}


ShapeBase *ShapeFactory::create_shape(const QString &shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const
{
    auto it = m_shapes_decl.find(shape_name);
    if(it != m_shapes_decl.end())
    {
        ShapeBase *shape_base =  it.value()->factory()->create_shape(shape_name, param_mgr, params, parent_attach_point);
        shape_base->m_shape_name = shape_name;
        return shape_base;
    }
    else
    {
        return nullptr;
    }
}

}
