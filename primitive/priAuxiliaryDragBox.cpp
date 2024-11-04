#include "priAuxiliaryDragBox.h"
#include "AttachTreeNode.h"
#include "attachTreeNodeMgr.h"
#include "qgraphicsscene.h"
#include "qgraphicssceneevent.h"
#include "qgraphicsview.h"
#include "qpainter.h"
#include "shapeBase.h"
#include "shapeDefines.h"
#include "QLineF"
#include <PriUtils.h>
#include <cmath>
#include <qmessagebox.h>
#include "paramUtils.h"
#include "viewport.h"

using namespace pr;

#define CIRCLE_RADIUS 10.0

namespace
{

// 计算直线 ab 上距离点 p 最近的点
QPointF closest_point_on_line(const QPointF &a, const QPointF &b, const QPointF &p)
{
    // 向量 AB
    QPointF AB = b - a;
    // 向量 AP
    QPointF AP = p - a;

    // 计算向量 AP 在向量 AB 上的投影
    qreal ab2 = AB.x() * AB.x() + AB.y() * AB.y(); // |AB|^2
    qreal ap_ab = AP.x() * AB.x() + AP.y() * AB.y(); // AP · AB
    qreal t = ap_ab / ab2;

    // 计算最近点的坐标
    QPointF closestPoint = a + t * AB;
    return closestPoint;
}

bool is_point_in_polygon(const QPointF &point, const QPolygonF &polygon)
{
    int intersections = 0;
    int n = polygon.size();

    for (int i = 0; i < n; ++i)
    {
        QPointF p1 = polygon[i];
        QPointF p2 = polygon[(i + 1) % n];

        // 检查点是否在多边形的边上
        if ((point.y() > p1.y()) != (point.y() > p2.y()) &&
                (point.x() < (p2.x() - p1.x()) * (point.y() - p1.y()) / (p2.y() - p1.y()) + p1.x()))
        {
            intersections++;
        }
    }

    // 如果交点数为奇数，则点在多边形内；否则在多边形外
    return (intersections % 2) != 0;
}


qreal point_to_line_distance(const QPointF &p, const QPointF &a, const QPointF &b)
{
    qreal A = p.x() - a.x();
    qreal B = p.y() - a.y();
    qreal C = b.x() - a.x();
    qreal D = b.y() - a.y();

    qreal dot = A * C + B * D;
    qreal len_sq = C * C + D * D;
    qreal param = (len_sq != 0) ? dot / len_sq : -1;

    qreal xx, yy;

    if (param < 0)
    {
        xx = a.x();
        yy = a.y();
    }
    else if (param > 1)
    {
        xx = b.x();
        yy = b.y();
    }
    else
    {
        xx = a.x() + param * C;
        yy = a.y() + param * D;
    }

    qreal dx = p.x() - xx;
    qreal dy = p.y() - yy;
    return std::sqrt(dx * dx + dy * dy);
}
}

PriAuxiliaryDragBox::PriAuxiliaryDragBox(at::AttachTreeNode* tree_node)
    : QGraphicsItem()
{
    set_tree_node(tree_node);
    // 启用悬停事件
    setAcceptHoverEvents(true);
    setZValue(1.0);
}

PriAuxiliaryDragBox::~PriAuxiliaryDragBox()
{

}

void PriAuxiliaryDragBox::set_tree_node(at::AttachTreeNode *tree_node)
{
    if(nullptr == tree_node)           // 情况1:对节点进行重置
    {
        mp_tree_node = nullptr;
    }
    else
    {
        const auto& shape_type = tree_node->shape_name();
        if(
            shape_type != SHAPE_CIRCLE &&
            shape_type != SHAPE_ELLIPSE &&
            shape_type != SHAPE_QUADRANGLE &&
            shape_type != SHAPE_RECTANGLE &&
            shape_type != SHAPE_SECTOR &&
            shape_type != SHAPE_TRIANGLE)  // 情况2：设置了无法处理的节点，视为设置为空节点
        {
            set_tree_node(nullptr);
        }
        else // 情况3：设置了有效节点
        {
            mp_tree_node = tree_node;
        }
    }
    update();
}

at::AttachTreeNode *pr::PriAuxiliaryDragBox::tree_node() const
{
    return mp_tree_node;
}

bool pr::PriAuxiliaryDragBox::set_cursor_in_dragging_circle(const QPointF &cursor_pos)
{
    // 设置鼠标样式为手形
    if(nullptr == mp_tree_node)
    {
        return false;
    }

    double rotate_angle = 0.0;
    const auto& params = mp_tree_node->params();
    for(const auto& param : params)
    {
        if(param.key() == SHAPE_ROTATE)
        {
            rotate_angle = param.to_double(mp_tree_node->param_mgr());
        }
    }

    bool is_in_right = direction_is_valid(MoveDirection::RIGHT) && direction_circle_area(MoveDirection::RIGHT).contains(cursor_pos);
    bool is_in_top = direction_is_valid(MoveDirection::TOP) && direction_circle_area(MoveDirection::TOP).contains(cursor_pos);

    QCursor angle_cursor = Qt::ArrowCursor;
    if(is_in_right)
    {
        angle_cursor = create_arrow_cursor(-rotate_angle + 90.0);
        scene()->views()[0]->setCursor(angle_cursor);
        // 按下左键时，设置拖拽模式为橡皮筋拖拽模式
        scene()->views()[0]->setDragMode(QGraphicsView::NoDrag);
    }
    else if(is_in_top)
    {
        angle_cursor = create_arrow_cursor(-rotate_angle);
        scene()->views()[0]->setCursor(angle_cursor);
        // 按下左键时，设置拖拽模式为橡皮筋拖拽模式
        scene()->views()[0]->setDragMode(QGraphicsView::NoDrag);
    }
    else
    {
        scene()->views()[0]->setCursor(angle_cursor);
        // 按下左键时，设置拖拽模式为橡皮筋拖拽模式
        scene()->views()[0]->setDragMode(QGraphicsView::RubberBandDrag);
        return false;
    }

    return true;
}

bool pr::PriAuxiliaryDragBox::has_started() const
{
    return m_has_started;
}

void pr::PriAuxiliaryDragBox::set_has_started(bool has_started)
{
    m_has_started = has_started;
}

void pr::PriAuxiliaryDragBox::set_started_point(const QPointF &cursor_pos)
{
    if(nullptr == mp_tree_node || false == m_has_started)
    {
        return;
    }

    if(direction_is_valid(MoveDirection::RIGHT) && direction_circle_area(MoveDirection::RIGHT).contains(cursor_pos))
    {
        m_moving_info.move_direction = MoveDirection::RIGHT;
    }
    else if(direction_is_valid(MoveDirection::TOP) && direction_circle_area(MoveDirection::TOP).contains(cursor_pos))
    {
        m_moving_info.move_direction = MoveDirection::TOP;
    }
}

bool pr::PriAuxiliaryDragBox::is_moveing() const
{
    return m_is_moving;
}

void pr::PriAuxiliaryDragBox::set_is_moving(bool is_moving)
{
    m_is_moving = is_moving;
}

void pr::PriAuxiliaryDragBox::set_current_moving_pos(const QPointF &cursor_pos)
{
    if(nullptr == mp_tree_node || !m_has_started || !m_is_moving)
    {
        return;
    }

    auto point_items = mp_tree_node->point_items();

    QPolygonF polygon;
    polygon << point_items[0]->pos() << point_items[1]->pos()  << point_items[2]->pos()   << point_items[3]->pos()  ;

    auto p = PriUtils::closest_grid_point(cursor_pos, mp_tree_node);
    p = mp_tree_node->tree_node_mgr()->viewport()->map_to_scene(p);

    m_moving_info.distance_positive  = !is_point_in_polygon(p, polygon);

    QPointF a;
    QPointF b;

    if(m_moving_info.move_direction  == MoveDirection::RIGHT)
    {
        a = point_items[2]->pos();
        b = point_items[3]->pos();
    }
    else if( m_moving_info.move_direction  == MoveDirection::TOP)
    {
        a = point_items[3]->pos();
        b = point_items[0]->pos();
    }

    QPointF closest_point = closest_point_on_line(a, b, p);
    m_moving_info.offset = p - closest_point;
}

void pr::PriAuxiliaryDragBox::reset()
{
    mp_tree_node = nullptr;
    m_has_started = false;
    m_is_moving = false;
    m_moving_info.move_direction = MoveDirection::NONE;
    m_moving_info.offset = {0, 0};
    m_moving_info.distance_positive = false;
}

void PriAuxiliaryDragBox::use_modification()
{
    if(nullptr == mp_tree_node || !m_has_started || !m_is_moving)
    {
        return;
    }

    qreal distance = std::sqrt(std::pow(m_moving_info.offset.x(), 2) + std::pow(m_moving_info.offset.y(), 2)) * (m_moving_info.distance_positive ? 1 : -1);
    distance =  mp_tree_node->tree_node_mgr()->viewport()->map_from_scene(distance);
    const auto& shape_type = mp_tree_node->shape_name();
    if(shape_type == SHAPE_CIRCLE)
    {
        auto params = mp_tree_node->params();
        for(auto& param : params)
        {
            if(param.key() == SHAPE_CIRCLE_RADIUS)
            {
                qreal radius = param.value();
                qreal diameter = radius * 2 + distance;
                radius = diameter * 0.5;
                pm::ParamUtils::modify_paramDecl(param, radius);
                break;
            }
        }
        mp_tree_node->set_params(params);
    }
    else if(shape_type == SHAPE_ELLIPSE)
    {
        auto params = mp_tree_node->params();
        for(auto& param : params)
        {
            if(m_moving_info.move_direction == MoveDirection::RIGHT && param.key() == SHAPE_ELLIPSE_SEMI_MAJOR_AXIS)
            {
                qreal semi_major_axis = param.value();
                semi_major_axis += distance / 2;
                pm::ParamUtils::modify_paramDecl(param, semi_major_axis);
                break;
            }
            else if(m_moving_info.move_direction == MoveDirection::TOP && param.key() == SHAPE_ELLIPSE_SEMI_MINOR_AXIS)
            {
                qreal semi_minor_axis = param.value();
                semi_minor_axis += distance / 2;
                pm::ParamUtils::modify_paramDecl(param, semi_minor_axis);
                break;
            }
        }
        mp_tree_node->set_params(params);
    }
    else if(shape_type == SHAPE_QUADRANGLE)
    {
        auto params = mp_tree_node->params();
        for(auto& param : params)
        {
            if(m_moving_info.move_direction == MoveDirection::RIGHT && param.key() == SHAPE_QUADRANGLE_WIDTH)
            {
                qreal width = param.value();
                width += distance;
                pm::ParamUtils::modify_paramDecl(param, width);
                break;
            }
            else if(m_moving_info.move_direction == MoveDirection::TOP && param.key() == SHAPE_QUADRANGLE_HEIGHT)
            {
                qreal height = param.value();
                height += distance;
                pm::ParamUtils::modify_paramDecl(param, height);
                break;
            }
        }
        mp_tree_node->set_params(params);
    }
    else if(shape_type == SHAPE_RECTANGLE)
    {
        auto params = mp_tree_node->params();
        for(auto& param : params)
        {
            if(m_moving_info.move_direction == MoveDirection::RIGHT && param.key() == SHAPE_RECTANGLE_WIDTH)
            {
                qreal width = param.value();
                width += distance;
                pm::ParamUtils::modify_paramDecl(param, width);
                break;
            }
            else if(m_moving_info.move_direction == MoveDirection::TOP && param.key() == SHAPE_RECTANGLE_HEIGHT)
            {
                qreal height = param.value();
                height += distance;
                pm::ParamUtils::modify_paramDecl(param, height);
                break;
            }
        }
        mp_tree_node->set_params(params);
    }
    else if(shape_type == SHAPE_SECTOR)
    {
        auto params = mp_tree_node->params();
        qreal factor = 0.0;
        for(auto& param : params)
        {
            if( param.key() == SHAPE_SECTOR_OUTSIDE_RADIUS)
            {
                qreal outside_radius = param.value();
                factor = (2 * outside_radius + distance) / (2 * outside_radius);
                break;
            }
        }

        for(auto& param : params)
        {
            if(param.key() == SHAPE_SECTOR_OUTSIDE_RADIUS)
            {
                qreal outside_radius = param.value();
                outside_radius *= factor;
                pm::ParamUtils::modify_paramDecl(param, outside_radius);
            }
            else if(param.key() == SHAPE_SECTOR_INSIDE_RADIUS)
            {
                qreal inside_radius = param.value();
                inside_radius *= factor;
                pm::ParamUtils::modify_paramDecl(param, inside_radius);
            }
        }
        mp_tree_node->set_params(params);
    }
    else if(shape_type == SHAPE_TRIANGLE)
    {
        auto params = mp_tree_node->params();
        for(auto& param : params)
        {
            if(m_moving_info.move_direction == MoveDirection::RIGHT && param.key() == SHAPE_TRIANGLE_WIDTH)
            {
                qreal width = param.value();
                width += distance;
                pm::ParamUtils::modify_paramDecl(param, width);
                break;
            }
            else if(m_moving_info.move_direction == MoveDirection::TOP && param.key() == SHAPE_TRIANGLE_HEIGHT)
            {
                qreal height = param.value();
                height += distance;
                pm::ParamUtils::modify_paramDecl(param, height);
                break;
            }
        }
        mp_tree_node->set_params(params);
    }
    mp_tree_node->tree_node_mgr()->update();
}

void PriAuxiliaryDragBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(!mp_tree_node)
    {
        return;
    }

    QPolygonF polygon;
    const auto& point_items = mp_tree_node->shape()->point_graphics_items();
    QVector<QPointF> points;
    for(auto point_item : point_items)
    {
        points.append(point_item->pos());
    }

    if(points.size() != 4)
    {
        return;
    }

    if(m_is_moving)
    {
        if(m_moving_info.move_direction == MoveDirection::RIGHT)
        {
            points[2] += m_moving_info.offset;
            points[3] += m_moving_info.offset;
        }
        else if( m_moving_info.move_direction == MoveDirection::TOP)
        {
            points[3] += m_moving_info.offset;
            points[0] += m_moving_info.offset;
        }
    }

    for(const auto& point : points)
    {
        polygon << point;
    }

    // 绘制虚线箭头
    QPen pen(Qt::darkGray, 3, Qt::DashLine);
    painter->setPen(pen);
    painter->drawPolygon(polygon);

    auto draw_circle = [&](const QPointF& mid_point)
    {
        pen.setWidth(2);

        // 设置点的边框颜色和填充颜色
        QBrush brush(Qt::white);
        painter->setPen(pen);
        painter->setBrush(brush);

        // 绘制空心点
        painter->drawEllipse(mid_point, CIRCLE_RADIUS, CIRCLE_RADIUS);
    };

    if(direction_is_valid(MoveDirection::RIGHT))
    {
        draw_circle((points[2] + points[3]) / 2);
    }

    if(direction_is_valid(MoveDirection::TOP))
    {
        draw_circle((points[3] + points[0]) / 2);
    }
}

QRectF PriAuxiliaryDragBox::boundingRect() const
{
    QRectF rect{};

    if(mp_tree_node)
    {
        rect = mp_tree_node->shape_item()->boundingRect();
    }

    return rect;
}

bool pr::PriAuxiliaryDragBox::direction_is_valid(MoveDirection direction) const
{
    if(nullptr == mp_tree_node)
    {
        return false;
    }

    if(direction == MoveDirection::NONE)
    {
        return false;
    }

    const auto& shape_type = mp_tree_node->shape_name();
    if(shape_type == SHAPE_CIRCLE)
    {
        auto params = mp_tree_node->params();
        for(auto& param : params)
        {
            if(param.key() == SHAPE_CIRCLE_RADIUS)
            {
                if(!pm::Expression::is_num(param.expression()))
                {
                    return false;
                }
            }
        }
    }
    else if(shape_type == SHAPE_ELLIPSE)
    {
        auto params = mp_tree_node->params();
        for(auto& param : params)
        {
            if(direction == MoveDirection::RIGHT && param.key() == SHAPE_ELLIPSE_SEMI_MAJOR_AXIS)
            {
                if(!pm::Expression::is_num(param.expression()))
                {
                    return false;
                }
            }
            else if(direction == MoveDirection::TOP && param.key() == SHAPE_ELLIPSE_SEMI_MINOR_AXIS)
            {
                if(!pm::Expression::is_num(param.expression()))
                {
                    return false;
                }
            }
        }
    }
    else if(shape_type == SHAPE_QUADRANGLE)
    {
        auto params = mp_tree_node->params();
        for(auto& param : params)
        {
            if(direction == MoveDirection::RIGHT && param.key() == SHAPE_QUADRANGLE_WIDTH)
            {
                if(!pm::Expression::is_num(param.expression()))
                {
                    return false;
                }
            }
            else if(direction == MoveDirection::TOP && param.key() == SHAPE_QUADRANGLE_HEIGHT)
            {
                if(!pm::Expression::is_num(param.expression()))
                {
                    return false;
                }
            }
        }
    }
    else if(shape_type == SHAPE_RECTANGLE)
    {
        auto params = mp_tree_node->params();
        for(auto& param : params)
        {
            if(direction == MoveDirection::RIGHT && param.key() == SHAPE_RECTANGLE_WIDTH)
            {
                if(!pm::Expression::is_num(param.expression()))
                {
                    return false;
                }
            }
            else if(direction == MoveDirection::TOP && param.key() == SHAPE_RECTANGLE_HEIGHT)
            {
                if(!pm::Expression::is_num(param.expression()))
                {
                    return false;
                }
            }
        }
    }
    else if(shape_type == SHAPE_SECTOR)
    {
        auto params = mp_tree_node->params();
        for(auto& param : params)
        {
            if( param.key() == SHAPE_SECTOR_OUTSIDE_RADIUS)
            {
                if(!pm::Expression::is_num(param.expression()))
                {
                    return false;
                }
            }
        }

        for(auto& param : params)
        {
            if(param.key() == SHAPE_SECTOR_OUTSIDE_RADIUS)
            {
                if(!pm::Expression::is_num(param.expression()))
                {
                    return false;
                }
            }
            else if(param.key() == SHAPE_SECTOR_INSIDE_RADIUS)
            {
                if(!pm::Expression::is_num(param.expression()))
                {
                    return false;
                }
            }
        }
    }
    else if(shape_type == SHAPE_TRIANGLE)
    {
        auto params = mp_tree_node->params();
        for(auto& param : params)
        {
            if(direction == MoveDirection::RIGHT && param.key() == SHAPE_TRIANGLE_WIDTH)
            {
                if(!pm::Expression::is_num(param.expression()))
                {
                    return false;
                }
            }
            else if(direction == MoveDirection::TOP && param.key() == SHAPE_TRIANGLE_HEIGHT)
            {
                if(!pm::Expression::is_num(param.expression()))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

QRectF pr::PriAuxiliaryDragBox::direction_circle_area(MoveDirection direction) const
{
    if(!direction_is_valid(direction))
    {
        return QRectF{};
    }

    // 设置点的大小和位置（以视图坐标系为基准）
    QPointF mid_point = mid_point_in_direction(direction);

    int x = mapFromScene(mid_point).x();
    int y = mapFromScene(mid_point).y();

    QRectF rect(x - CIRCLE_RADIUS, y - CIRCLE_RADIUS, CIRCLE_RADIUS * 2, CIRCLE_RADIUS * 2);
    return rect;
}

QPointF PriAuxiliaryDragBox::mid_point_in_direction(MoveDirection direction) const
{
    if(nullptr == mp_tree_node)
    {
        return QPointF{};
    }

    const auto& point_items = mp_tree_node->shape()->point_graphics_items();
    QVector<QPointF> points;
    for(auto point_item : point_items)
    {
        points.append(point_item->pos());
    }

    if(points.size() != 4)
    {
        return QPointF{};
    }

    if(direction == MoveDirection::RIGHT)
    {
        return (points[2] + points[3]) * 0.5;
    }
    else if(direction == MoveDirection::TOP)
    {
        return (points[3] + points[0]) * 0.5;
    }

    return QPointF{};
}

QCursor pr::PriAuxiliaryDragBox::create_arrow_cursor(qreal angle)
{
    // 创建一个空的 QPixmap
    int width = 32;
    int height = 32;
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);

    // 手动设置像素来绘制双向箭头
    QImage image(32, 32, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    // 设置箭头的颜色
    QRgb arrowColor = qRgba(0, 0, 0, 255);

    // 绘制朝上的箭头
    for (int y = 0; y < 10; ++y)
    {
        for (int x = -y; x <= y; ++x)
        {
            image.setPixel(width / 2 + x, y, arrowColor);
        }
    }

    // 绘制朝下的箭头
    for (int y = 0; y < 10; ++y)
    {
        for (int x = -y; x <= y; ++x)
        {
            image.setPixel(width / 2 + x, height - 1 - y, arrowColor);
        }
    }

    // 绘制中间的竖线
    for (int y = 10; y < height - 10; ++y)
    {
        image.setPixel(width / 2, y, arrowColor);
        image.setPixel(width / 2 - 1, y, arrowColor);
        image.setPixel(width / 2 + 1, y, arrowColor);
    }

    // 将 QImage 转换为 QPixmap
    pixmap = QPixmap::fromImage(image);

    // 旋转箭头
    QTransform transform;
    transform.rotate(angle);
    pixmap = pixmap.transformed(transform);

    // 创建自定义光标
    return QCursor(pixmap);
}
