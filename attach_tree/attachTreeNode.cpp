#include <AttachTreeRootNode.h>
#include <AttachTreeUtils.h>
#include <PointE.h>
#include <QGraphicsView>
#include <ShapeDecl.h>
#include <viewport.h>

#include "attachTreeNode.h"
#include "attachTreeDefines.h"

#include "shapeBase.h"
#include "shapeFactory.h"
#include "shapePointGraphicsItem.h"
// #include "redoService.h"
#include "attachTreeNodeMgr.h"
#include "ParamDecl.h"

#include "shapeDefines.h"
#include "expression.h"

using namespace at;

AttachTreeNode::AttachTreeNode(AttachTreeBaseNode* parent,
                               int parent_attach_point_idx,
                               const QString& shape_name,
                               const QVector<pm::ParamDecl> & params,
                               NodeType node_type,
                               NodeDirection node_dirction,
                               NodeBooleanSubtractType node_boolean_subtra_type,
                               ly::LayerInfo* layer,
                               bool connect_with_mw)
    : AttachTreeBaseNode()
    , mp_parent(parent)
    , m_parent_attach_point_idx(parent_attach_point_idx)
    , m_shape_name(shape_name)
    , m_node_type(node_type)
    , m_node_direction(node_dirction)
    , m_node_bool_subtraction(node_boolean_subtra_type)
    , mp_layer_info(layer)
    , m_connect_with_mw(connect_with_mw)
{
    // 创建Shape
    sp::ShapePointGraphicsItem* parent_attach_point = nullptr;
    if(dynamic_cast<AttachTreeRootNode * >(mp_parent))
    {
        parent_attach_point = dynamic_cast<AttachTreeRootNode*>(mp_parent)->origin_point();
    }
    else
    {
        parent_attach_point = dynamic_cast<AttachTreeNode*>(mp_parent)->point_items()[parent_attach_point_idx];
    }

    mp_shape = sp::ShapeFactory::instance()->create_shape(shape_name, mp_parent->param_mgr(), params, parent_attach_point);
    mp_shape->shape_graphics_item()->set_layer_info(mp_layer_info);
    if(mp_parent->view())
    {
        mp_shape->update();
    }

    // 分配Id
    m_tree_node_id = parent->tree_node_mgr()->register_tree_node(this);

    connect(mp_parent->param_mgr(), &pm::ParamMgr::param_changed, this, &AttachTreeNode::on_param_changed);
}

AttachTreeNode::~AttachTreeNode()
{
    auto pm_mgr = param_mgr();

    // 把全部子对象都析构
    remove_all();

    // 从自己的父亲中移出自己
    mp_parent->remove_child(this);

    // 处理自身的对象
    auto* shape_item = mp_shape->shape_graphics_item();
    if (shape_item)
    {
        shape_item->setVisible(false);
        shape_item->setSelected(false);
        shape_item->update();
        if(view())
        {
            view()->scene()->removeItem(shape_item);
        }
        auto point_items = mp_shape->point_graphics_items();

        for (auto point_item : point_items)
        {
            point_item->setVisible(false);
            point_item->setSelected(false);
            point_item->update();
            if(view())
            {
                view()->scene()->removeItem(point_item);
            }
        }
    }
    SAFE_DELETE(mp_shape);

    tree_node_mgr()->unregister_tree_node(m_tree_node_id);

    disconnect(pm_mgr, &pm::ParamMgr::param_changed, this, &AttachTreeNode::on_param_changed);
}

sp::ShapeDrawGraphicsItem* AttachTreeNode::shape_item()
{
    return mp_shape->shape_graphics_item();
}

QVector<sp::ShapePointGraphicsItem*> at::AttachTreeNode::point_items() const
{
    return mp_shape->point_graphics_items();
}

void AttachTreeNode::set_node_type(NodeType node_type)
{
    auto old_node_type = m_node_type;

    if(m_node_type == node_type)
    {
        return;
    }

    m_node_type = node_type;

    //for redo
    // cm::RedoService::instance()->queue(this, new SetNodeTypeOp(old_node_type, m_node_type));
}

NodeType AttachTreeNode::node_type() const
{
    return m_node_type;
}

void AttachTreeNode::set_node_direction(NodeDirection node_direction)
{
    auto old_dire = m_node_direction;

    if(m_node_direction == node_direction)
    {
        return;
    }

    m_node_direction = node_direction;

    //for redo
    // cm::RedoService::instance()->queue(this, new SetDirectionOp(old_dire, m_node_direction));
}

NodeDirection AttachTreeNode::node_direction() const
{
    return m_node_direction;
}

void AttachTreeNode::set_node_boolean_subtract_type(NodeBooleanSubtractType node_direction)
{
    m_node_bool_subtraction = node_direction;
}

NodeBooleanSubtractType AttachTreeNode::node_boolean_subtract_type() const
{
    return m_node_bool_subtraction;
}

void AttachTreeNode::set_layer_info(ly::LayerInfo* layer_info)
{
    auto old_layer = mp_layer_info;

    if(mp_layer_info == layer_info)
    {
        return;
    }

    mp_layer_info = layer_info;
    shape_item()->set_layer_info(mp_layer_info);

    //for redo
    // cm::RedoService::instance()->queue(this, new SetLayerOp(old_layer, mp_layer_info));
}

ly::LayerInfo* AttachTreeNode::layer_info() const
{
    return mp_layer_info;
}

const QVector<pm::ParamDecl> &AttachTreeNode::params() const
{
    return mp_shape->params();
}

AttachTreeNodeId AttachTreeNode::id() const
{
    return m_tree_node_id;
}

void AttachTreeNode::set_new_id()
{
    Q_ASSERT(m_tree_node_id != INVALID_ATTACH_TREE_NODE_ID);
    m_tree_node_id = tree_node_mgr()->allocate_tree_node_new_id(m_tree_node_id);
}

bool at::AttachTreeNode::connect_with_mw() const
{
    return m_connect_with_mw;
}

void AttachTreeNode::set_connect_with_mw(bool connect_with_mw)
{
    m_connect_with_mw = connect_with_mw;
}

void at::AttachTreeNode::on_param_changed(const QString &key)
{
    if(!mp_shape->param_used(key))return;

    update();
}

void AttachTreeNode::set_params(const QVector<pm::ParamDecl> & params)
{
    auto old_params = mp_shape->params();
    mp_shape->set_new_params(params);
    update();
    //for redo
    // cm::RedoService::instance()->queue(this, new SetParamsOp(old_params, params));
}

void AttachTreeNode::update()
{
    auto graphics_view = view();
    if(nullptr == graphics_view)
    {
        return;
    }

    // 将QGraphicsItem加入View中
    if(graphics_view->scene() != shape_item()->scene())  // 避免重复添加
    {
        graphics_view->scene()->addItem(shape_item());
        const auto& pt_items = point_items();
        for(auto pt_item : pt_items)
        {
            graphics_view->scene()->addItem(pt_item);
        }
    }

    /* 更新内容：
     * 1. 变换矩阵；
     * 2. 参数和附着点等的数据；
     * 3. 更新子节点;
     */
    update_trans();
    mp_shape->update();

    if(!update_children())
    {
        return;
    }

    const auto& child_items = children();
    for (auto itor = child_items.begin(); itor != child_items.end(); itor++)
    {
        for (auto & node : itor.value())
        {
            if (node)
            {
                node->update();
            }
        }
    }
}

void AttachTreeNode::update_trans()
{
    // 设置附着点相对局部坐标系的相对位置的偏移
    auto tree_node = dynamic_cast<AttachTreeNode*>(mp_parent);
    if(tree_node)
    {
        QPointF at_pt = AttachTreeUtils::attach_point_coord(tree_node, get_parent_attach_point_idx(), false);
        QTransform trans = mp_parent->transform();
        trans.translate(at_pt.x(), at_pt.y());
        set_transform(trans);
    }
    else
    {
        QTransform trans;
        set_transform(trans);
    }

    // attch point exp
    if(tree_node)
    {
        pm::PointE point = AttachTreeUtils::attach_exp_point_coord(tree_node, get_parent_attach_point_idx(), false);
        auto exp_trans = mp_parent->transorm_for_expression();
        exp_trans.translate(point.x(), point.y());
        set_transform_for_expression(exp_trans);
    }
    else
    {
        pm::PointE exp_p("0", "0");
        set_transform_for_expression(exp_p);
    }
}

#if 0
void at::AttachTreeNode::undo(db::Op *op)
{
    SetParamsOp  *p_op = dynamic_cast<SetParamsOp*>(op);
    if(p_op)
    {
        auto params = p_op->old_params();
        mp_shape->set_new_params(params);
    }

    SetLayerOp *l_op = dynamic_cast<SetLayerOp*>(op);
    if(l_op)
    {
        mp_layer_info = l_op->old_layer();
        shape_item()->set_layer_info(mp_layer_info);
    }
    SetNodeTypeOp *t_op = dynamic_cast<SetNodeTypeOp*>(op);
    if(t_op)
    {
        m_node_type = t_op->old_type();
    }
    SetDirectionOp *d_op = dynamic_cast<SetDirectionOp*>(op);
    if(d_op)
    {
        m_node_direction = d_op->old_dire();
    }

    update();
    tree_node_mgr()->update();
}

void AttachTreeNode::redo(db::Op *op)
{
    SetParamsOp  *p_op = dynamic_cast<SetParamsOp*>(op);
    if(p_op)
    {
        auto params = p_op->new_params();
        mp_shape->set_new_params(params);
    }

    SetLayerOp *l_op = dynamic_cast<SetLayerOp*>(op);
    if(l_op)
    {
        mp_layer_info = l_op->new_layer();
        shape_item()->set_layer_info(mp_layer_info);
    }
    SetNodeTypeOp *t_op = dynamic_cast<SetNodeTypeOp*>(op);
    if(t_op)
    {
        m_node_type = t_op->new_type();
    }
    SetDirectionOp *d_op = dynamic_cast<SetDirectionOp*>(op);
    if(d_op)
    {
        m_node_direction = d_op->new_dire();
    }

    update();
    tree_node_mgr()->update();
}
#endif

pm::ParamMgr *at::AttachTreeNode::param_mgr() const
{
    return mp_parent->param_mgr();
}

QGraphicsView *at::AttachTreeNode::view() const
{
    return mp_parent->view();
}

AttachTreeNodeMgr *at::AttachTreeNode::tree_node_mgr() const
{
    return mp_parent->tree_node_mgr();
}

AttachTreeBaseNode *AttachTreeNode::parent_node() const
{
    return mp_parent;
}

void AttachTreeNode::set_new_parent_node(AttachTreeBaseNode *new_parent, int parent_index)
{
    mp_parent = new_parent;
    m_parent_attach_point_idx = parent_index;
    if (!new_parent)
        return;

    // 创建Shape
    sp::ShapePointGraphicsItem* parent_attach_point = nullptr;
    if(dynamic_cast<AttachTreeRootNode * >(mp_parent))
    {
        parent_attach_point = dynamic_cast<AttachTreeRootNode*>(mp_parent)->origin_point();
    }
    else
    {
        parent_attach_point = dynamic_cast<AttachTreeNode*>(mp_parent)->point_items()[m_parent_attach_point_idx];
    }
    mp_shape->set_parent_attach_point(parent_attach_point);

    // new_parent->add_child(this, parent_index); 这件事不在此处理
}

int AttachTreeNode::get_parent_attach_point_idx() const
{
    return m_parent_attach_point_idx;
}

sp::ShapeBase* AttachTreeNode::shape() const
{
    return mp_shape;
}

void AttachTreeNode::set_anchor_point(int attach_point_idx,
                                      bool is_anchor)
{
    QVector<sp::ShapePointGraphicsItem *>   point_items = mp_shape->point_graphics_items();
    Q_ASSERT( point_items.size() > attach_point_idx);
    point_items[attach_point_idx]->set_anchor_point(is_anchor);
}

bool AttachTreeNode::is_anchor_point(int attach_point_idx)
const
{
    QVector<sp::ShapePointGraphicsItem *> point_items = mp_shape->point_graphics_items();
    Q_ASSERT( point_items.size() > attach_point_idx);
    return point_items[attach_point_idx]->is_anchor_point();
}

void AttachTreeNode::set_coord_point(int attach_point_idx, bool is_coord)
{
    QVector<sp::ShapePointGraphicsItem *> point_items = mp_shape->point_graphics_items();
    Q_ASSERT( point_items.size() > attach_point_idx);
    point_items[attach_point_idx]->set_coor_anchor_point(is_coord);
}

bool AttachTreeNode::is_coord_point(int attach_point_idx) const
{
    QVector<sp::ShapePointGraphicsItem *> point_items = mp_shape->point_graphics_items();
    Q_ASSERT( point_items.size() > attach_point_idx);
    return point_items[attach_point_idx]->is_coor_anchor_point();
}

const QString &AttachTreeNode::shape_name() const
{
    return m_shape_name;
}

void at::AttachTreeNode::set_shape_name(const QString &new_shape_name)
{
    if(m_shape_name == new_shape_name ||  m_shape_name != SHAPE_RECTANGLE || m_node_type != NodeType::LOCATION || mp_shape == nullptr)
    {
        return;
    }

    auto old_shape = mp_shape;
    const QVector<pm::ParamDecl>& old_params = old_shape->params();
    QString old_rectangle_width;
    QString old_rectangle_height;
    QString old_rectangle_rotate;
    for(const auto& param : old_params)
    {
        const auto& key = param.key();
        const auto& exp = param.expression();
        if(key == SHAPE_RECTANGLE_WIDTH)
        {
            old_rectangle_width = exp;
        }
        else if(key == SHAPE_RECTANGLE_HEIGHT)
        {
            old_rectangle_height = exp;
        }
        else if(key == SHAPE_ROTATE)
        {
            old_rectangle_rotate = exp;
        }
        else
        {
            Q_ASSERT(false);
        }
    }

    double width = pm::Expression(old_rectangle_width).to_double(old_shape->param_mgr());
    double height = pm::Expression(old_rectangle_height).to_double(old_shape->param_mgr());

    bool is_width_num = pm::Expression::is_num(old_rectangle_width);
    bool is_height_num = pm::Expression::is_num(old_rectangle_height);

    QString priority_parameter_expression;
    if(is_width_num && is_height_num)
    {
        priority_parameter_expression = (width > height ? old_rectangle_width : old_rectangle_height);
    }
    else if(!is_width_num && is_height_num)
    {
        priority_parameter_expression = old_rectangle_width;
    }
    else if(is_width_num && !is_height_num)
    {
        priority_parameter_expression = old_rectangle_height;
    }
    else if(!is_width_num && !is_height_num)
    {
        priority_parameter_expression = (width > height ? old_rectangle_width : old_rectangle_height);
    }


    QVector<pm::ParamDecl> params;
    if(new_shape_name == SHAPE_CIRCLE)
    {
        pm::Expression radius_exp = pm::Expression(priority_parameter_expression) * 0.5;
        pm::ParamDecl param_radius(SHAPE_CIRCLE_RADIUS, radius_exp.to_str());
        params.push_back(param_radius);
    }
    else if(new_shape_name == SHAPE_ELLIPSE)
    {
        pm::Expression semi_major_axis_exp = pm::Expression(old_rectangle_width) * 0.5;
        QString semi_major_axis = semi_major_axis_exp.to_str();
        pm::Expression semi_minor_axis_exp = pm::Expression(old_rectangle_height) * 0.5;
        QString semi_minor_axis = semi_minor_axis_exp.to_str();

        pm::ParamDecl param_semi_major_axis(SHAPE_ELLIPSE_SEMI_MAJOR_AXIS, semi_major_axis);
        pm::ParamDecl param_semi_minor_axis(SHAPE_ELLIPSE_SEMI_MINOR_AXIS, semi_minor_axis);
        pm::ParamDecl param_start_angle(SHAPE_ELLIPSE_START_ANGLE, 0);
        pm::ParamDecl param_end_angle(SHAPE_ELLIPSE_END_ANGLE, 0);

        params.push_back(param_semi_major_axis);
        params.push_back(param_semi_minor_axis);
        params.push_back(param_start_angle);
        params.push_back(param_end_angle);

    }
    else if(new_shape_name == SHAPE_QUADRANGLE)
    {
        pm::ParamDecl param_width(SHAPE_QUADRANGLE_WIDTH, old_rectangle_width);
        pm::ParamDecl param_height(SHAPE_QUADRANGLE_HEIGHT, old_rectangle_height);
        pm::ParamDecl param_parameter_1(SHAPE_QUADRANGLE_PARAMETER_1, 0);
        pm::ParamDecl param_parameter_2(SHAPE_QUADRANGLE_PARAMETER_2, 0);
        pm::ParamDecl param_parameter_3(SHAPE_QUADRANGLE_PARAMETER_3, 0);
        pm::ParamDecl param_parameter_4(SHAPE_QUADRANGLE_PARAMETER_4, 0);

        params.push_back(param_width);
        params.push_back(param_height);
        params.push_back(param_parameter_1);
        params.push_back(param_parameter_2);
        params.push_back(param_parameter_3);
        params.push_back(param_parameter_4);
    }
    else if(new_shape_name == SHAPE_SECTOR)
    {
        pm::Expression out_radius_exp = pm::Expression(priority_parameter_expression) * 0.5;

        pm::ParamDecl param_inside_radius(SHAPE_SECTOR_INSIDE_RADIUS, 0);
        pm::ParamDecl param_outside_radius(SHAPE_SECTOR_OUTSIDE_RADIUS, out_radius_exp.to_str());
        pm::ParamDecl param_start_angle(SHAPE_SECTOR_START_ANGLE, 0);
        pm::ParamDecl param_end_angle(SHAPE_SECTOR_END_ANGLE, 90);

        params.push_back(param_inside_radius);
        params.push_back(param_outside_radius);
        params.push_back(param_start_angle);
        params.push_back(param_end_angle);
    }
    else if(new_shape_name == SHAPE_TRIANGLE)
    {
        pm::ParamDecl param_width(SHAPE_TRIANGLE_WIDTH, old_rectangle_width);
        pm::ParamDecl param_height(SHAPE_TRIANGLE_HEIGHT, old_rectangle_height);

        params.push_back(param_width);
        params.push_back(param_height);
    }
    else
    {
        return;
    }
    pm::ParamDecl param_rotate(SHAPE_ROTATE, old_rectangle_rotate);
    params.push_back(param_rotate);

    auto new_shape = sp::ShapeFactory::instance()->create_shape(new_shape_name, mp_parent->param_mgr(), params, old_shape->parent_attach_point());

    new_shape->replace_graphics_items(old_shape);

    new_shape->shape_graphics_item()->set_layer_info(mp_layer_info);
    mp_shape = new_shape;
    m_shape_name = new_shape_name;

    SAFE_DELETE(old_shape);

    update();
}
