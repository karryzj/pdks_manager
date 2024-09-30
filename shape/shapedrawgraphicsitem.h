#ifndef SHAPEDRAWGRAPHICSITEM_H
#define SHAPEDRAWGRAPHICSITEM_H

#include <QObject>

#include <ParamDecl.h>
#include <ParamMgr.h>
#include "shapePointGraphicsItem.h"
#include "spCommon.h"

namespace ly
{
class LayerInfo;
}

namespace at
{
class AttachTreeNode;
}

namespace sp
{

class ShapeBase;

// 定义位运算符
enum class PaintType : quint8
{
    DASH = 0,               // 不考虑图层，只绘制虚线
    DRAW = 1,               // 绘制图形的全部内容
};

class SP_PUBLIC ShapeDrawGraphicsItem final: public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    ShapeDrawGraphicsItem(sp::ShapeBase* shape);
    ~ShapeDrawGraphicsItem() override;

    // 必须实现的虚接口
public:
    void set_painter_path(const QPainterPath& path);
    void set_paint_type(PaintType paint_type);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override final;
    QRectF boundingRect() const override final;
    // 一些触发事件
    sp::ShapeBase* shape_info() const;

    void set_point_items_visible(bool visible) const;

    // 公共的普通成员函数
public:
    // 获得当前ShapeDrawGraphicsItem实例的名字。实际的赋值过程在子类的构造中已经完成
    const QString& shape_name() const;
    void set_layer_info(const ly::LayerInfo* lay_info);
protected:
    void set_shape_name(const QString& shape_name);
    // 返回附着点
    QVector<ShapePointGraphicsItem*> get_point_items() const;
private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

signals:
    void item_double_clicked(ShapeDrawGraphicsItem* shape_item);

    // 发送给treewidget的信号
private:
    QString m_shape_item_name = QString();                  // 当前ShapeDrawGraphicsItem实例的名字
    sp::ShapeBase* mp_shape;
    QPainterPath m_actual_draw_path;
    const ly::LayerInfo* mp_layer_info = nullptr;
    PaintType m_paint_type = PaintType::DRAW;
};

}


#endif // SHAPEDRAWGRAPHICSITEM_H
