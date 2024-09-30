#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QLabel>
#include <QTimer>

namespace cm
{

class Viewport;

}

namespace at
{
class AttachTreeNodeMgr;
}

namespace  sp
{
class ShapePointGraphicsItem;
}

namespace pr
{
class PriGraphicsScene;
class Primitive;
class PriGraphicsView final: public QGraphicsView
{
    Q_OBJECT
public:
    PriGraphicsView(PriGraphicsScene *scene,  QWidget *parent = nullptr);
    ~PriGraphicsView(void) override;

    void set_primitive(Primitive* pri);
    Primitive* current_primitive() const;

public slots:
    void align_point_items(const QString& align_type);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    void mouseMoved(const QPointF &message);
    void mouseLeaveRightArea(void);

    void mouse_left_button_press(const QPoint &pos);
    void mouse_left_button_press_item(QGraphicsItem* item);

    void key_escape_press();

    void double_click_mouse_left_button();

    // 以下信号用于右键菜单
    void mouse_right_button_press(const QPoint& global_pos, QGraphicsItem* item);

private slots:
    void single_click_no_double_click();
private:
    void setup_point_item_param_by_pos(const QPointF& pos, sp::ShapePointGraphicsItem* point_item);

private:
    double m_scaleFactor;

    Primitive* mp_pri = nullptr;

    QVector<sp::ShapePointGraphicsItem*> m_cache_point_items;   // HINT@leixunyong。如果后续需要使用框选的对象，可以修改或者使用这里的成员变量。

    // HINT@leixunyong。用于区分单击和双击事件
    bool m_is_double_click = false;
    QTimer m_click_timer;
    QPoint m_cache_single_click_pos;
};
}
#endif // GRAPHICSVIEW_H
