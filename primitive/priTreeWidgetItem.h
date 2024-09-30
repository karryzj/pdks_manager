#ifndef PRITREEWIDGETITEM_H
#define PRITREEWIDGETITEM_H

#include <QTreeWidgetItem>

class QGraphicsItem;
namespace pr
{
class PriTreeWidgetItem final : public QTreeWidgetItem
{
public:
    PriTreeWidgetItem(QTreeWidgetItem *parent, QGraphicsItem* graphics_item);
    PriTreeWidgetItem(QTreeWidget *treeview, QGraphicsItem* graphics_item);
    ~PriTreeWidgetItem() override;

    QGraphicsItem* graphics_item() const;

private:
    void init_connect();

private slots:
    void setup_current_tree_widget_item(QTreeWidgetItem *item, int column);

private:
    QGraphicsItem* mp_graphics_item;

};
}



#endif // PRITREEWIDGETITEM_H
