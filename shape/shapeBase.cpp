
#include "shapeBase.h"
#include "common_defines.h"
#include "paramMgr.h"
#include "qdebug.h"
#include "qgraphicsscene.h"
#include "scopeTimer.h"
#include "shapeDefines.h"
#include "QPainterPath"

#include "pointE.h"

#include <ShapeUtils.h>

namespace sp
{
ShapeBase::ShapeBase(pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl>& params, ShapePointGraphicsItem* parent_attach_point)
    : QObject()
    , m_rotate(0)
    , m_rotate_str("0")
    , m_params(params)
    , mp_param_mgr(param_mgr)
    , m_coord_anchor_point(0, 0)
    , mp_parent_attach_point(parent_attach_point)
{
    static int idx = 0;
    idx++;

    m_id = idx;
    // 初始化shape item
    mp_shape_item = new ShapeDrawGraphicsItem(this);
    // 设置项的可选中性和可聚焦性
    mp_shape_item->setFlag(QGraphicsItem::ItemIsSelectable);
    mp_shape_item->setFlag(QGraphicsItem::ItemIsFocusable);

    m_attach_points_e.resize(4);
    m_attach_points.resize(4);
}

ShapeBase::~ShapeBase()
{
}

void ShapeBase::set_new_param_mgr(pm::ParamMgr *param_mgr)
{
    mp_param_mgr = param_mgr;
    update();
}

void ShapeBase::set_new_params(const QVector<pm::ParamDecl> &params)
{
    m_params = params;
    update();
}

const QVector<pm::ParamDecl> &ShapeBase::params()
{
    return m_params;
}

bool ShapeBase::param_used(const QString &key)
{
    for(auto param : m_params)
    {
        if(param.expression().contains(key))
        {
            return true;
        }
    }
    return false;
}

const QString &ShapeBase::shape_type()
{
    return m_shape_name;
}

QString ShapeBase::shape_name() const
{
    return m_shape_name + QString::number(m_id);
}

const QVector<ShapePointGraphicsItem *>& ShapeBase::point_graphics_items() const
{
    return m_point_items;
}

void ShapeBase::set_point_graphics_items(const QVector<ShapePointGraphicsItem *> &point_items)
{
    for(auto point_item : m_point_items)
    {
        SAFE_DELETE(point_item);
    }
    m_point_items.clear();

    m_point_items = point_items;
}

void ShapeBase::remove_point_graphics_items()
{
    m_point_items.clear();
}

void ShapeBase::set_shape_graphics_item(ShapeDrawGraphicsItem *shape_item)
{
    if(mp_shape_item && mp_shape_item->scene())
    {
        mp_shape_item->scene()->removeItem(mp_shape_item);
    }

    SAFE_DELETE(mp_shape_item);

    mp_shape_item = shape_item;
}

void ShapeBase::remove_shape_item()
{
    mp_shape_item = nullptr;
}

void ShapeBase::set_anchor_points(const QVector<QPointF>& points)
{
    m_anchor_points = points;
}

QVector<QPointF> ShapeBase::get_anchor_points()
{
    return m_anchor_points;
}

void ShapeBase::set_coord_anchor_point(const QPointF &point)
{
    m_coord_anchor_point = point;
}

QPointF ShapeBase::get_coord_anchor_point()
{
    return m_coord_anchor_point;
}

void ShapeBase::set_parent_attach_point(ShapePointGraphicsItem *point_item)
{
    mp_parent_attach_point = point_item;
}

ShapePointGraphicsItem *ShapeBase::parent_attach_point() const
{
    return mp_parent_attach_point;
}

double ShapeBase::rotate() const
{
    return m_rotate;
}

pm::Expression ShapeBase::rotate_exp() const
{
    return QString(m_rotate_str.c_str());
}

pm::ParamMgr *ShapeBase::param_mgr() const
{
    return mp_param_mgr;
}

void ShapeBase::update()
{
    update_variables();
    update_attach_points();
    update_path();
}

void ShapeBase::update_variables()
{
    for (auto i = m_params.begin(); i != m_params.end(); ++i)
    {
        pm::ParamDecl param = * i;
        if (param.key() == SHAPE_ROTATE)
        {
            m_rotate = param.to_float(mp_param_mgr);
            m_rotate_str = param.to_str().toStdString();
            break;  // HINT@leixunyong。此处的设计是更新相同的参数。如果后续有其他的参数，需要对此处进行额外处理。当前只有旋转是统一参数。
        }
    }
}

void ShapeBase::update_attach_points()
{
    //ex
    update_attach_pointes();
    //value
    if(m_attach_points.size() != m_attach_points_e.size())
    {
        m_attach_points.resize(m_attach_points_e.size());
    }

    for(auto idx = 0; idx < m_attach_points_e.size(); idx++)
    {
        auto& p = m_attach_points_e[idx];
        m_attach_points[idx] = QPointF(QPointF(p.x().to_double(mp_param_mgr), p.y().to_double(mp_param_mgr)));
    }
}

void ShapeBase::update_path()
{
    m_path = build_path();
}

const QVector<QPointF> &ShapeBase::attach_points()
{
    return m_attach_points;
}

const QVector<pm::PointE> &ShapeBase::attach_exp_points()
{
    return m_attach_points_e;
}

QPainterPath &ShapeBase::painter_path()
{
    return m_path;
}

ShapeDrawGraphicsItem *ShapeBase::shape_graphics_item() const
{
    Q_ASSERT( nullptr != mp_shape_item);
    return mp_shape_item;
}

void ShapeBase::replace_graphics_items(ShapeBase *other_shape)
{
    auto old_shape_item = other_shape->shape_graphics_item();
    this->set_shape_graphics_item(old_shape_item);
    other_shape->remove_shape_item();

    const auto& old_shape_point_items = other_shape->point_graphics_items();
    this->set_point_graphics_items(old_shape_point_items);
    other_shape->remove_point_graphics_items();

    old_shape_item->set_shape_info(this);
}
}
