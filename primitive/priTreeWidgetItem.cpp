#include "priTreeWidgetItem.h"
#include "priTreeWidgetMenu.h"

#include <priTreeWidget.h>

using namespace pr;

PriTreeWidgetItem::PriTreeWidgetItem(QTreeWidgetItem *parent, QGraphicsItem *graphics_item)
    : QTreeWidgetItem(parent)
    , mp_graphics_item(graphics_item)
{

}

PriTreeWidgetItem::PriTreeWidgetItem(QTreeWidget *treeview, QGraphicsItem *graphics_item)
    : QTreeWidgetItem(treeview)
    , mp_graphics_item(graphics_item)
{

}

PriTreeWidgetItem::~PriTreeWidgetItem()
{

}

QGraphicsItem *PriTreeWidgetItem::graphics_item() const
{
    return mp_graphics_item;
}
