#ifndef PRIRULERDIALOG_H
#define PRIRULERDIALOG_H

#include <QDialog>
#include <QGraphicsEllipseItem>

class QGraphicsItem;
namespace sp
{
class ShapePointGraphicsItem;
}

namespace pr
{
class Primitive;
class PriGraphicsView;
class ArrowLineItem;
class PriRulerDialog final : public QDialog
{
public:
    PriRulerDialog(Primitive* pri,  PriGraphicsView* view, QWidget *parent = nullptr);
    ~PriRulerDialog() override;

private:
    void init();
    void set_connect();
    void remove_connect();
    void closeEvent(QCloseEvent *event) override;
private slots:
    void setup_point_item(QGraphicsItem* item);

private:
    Primitive* mp_pri;
    PriGraphicsView* mp_view;

    sp::ShapePointGraphicsItem* mp_first_point_item = nullptr;
    sp::ShapePointGraphicsItem* mp_second_point_item = nullptr;

    ArrowLineItem* m_distance_arrow = nullptr;

    bool m_has_connects = false;
};

class ArrowLineItem final : public QGraphicsItem
{
    friend class PriRulerDialog;
private:
    ArrowLineItem(const QPointF &start, const QPointF &end, const QString &text, QGraphicsItem *parent = nullptr);
    ~ArrowLineItem() override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void draw_arrow(QPainter *painter, const QPointF &start, const QPointF &end);
    void draw_text_on_line(QPainter *painter, const QPointF &start, const QPointF &end, const QString &text);
private:
    QPointF m_start_point;
    QPointF m_end_point;
    QString m_text;


};
}



#endif // PRIRULERDIALOG_H
