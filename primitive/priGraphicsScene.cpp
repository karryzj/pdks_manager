#include "priGraphicsScene.h"
#include "attachTreeUtils.h"

#include <AttachTreeNode.h>
#include <QGraphicsSceneContextMenuEvent>
#include <QKeyEvent>
#include <ShapeDrawGraphicsItem.h>
#include <qmenu.h>
#include <QDebug>
#include <PriGraphicsView.h>
#include "primitive.h"
#include "primitiveDefines.h"
#include "ruler.h"
#include "viewport.h"
#include "priUtils.h"
#include "removeCommand.h"
#include "commandManager.h"

using namespace pr;
using namespace sp;

PriGraphicsScene::PriGraphicsScene(QObject *parent)
    : QGraphicsScene{parent}
    , mp_ruler(new cm::Ruler())
{
    setObjectName(tr(PRI_GRAPHICS_SCENE));

}

pr::PriGraphicsScene::~PriGraphicsScene()
{
}

void PriGraphicsScene::init()
{
    addItem(mp_ruler);
}

void PriGraphicsScene::set_ruler(double dbu, cm::Viewport *viewport)
{
    mp_ruler->set(dbu, viewport);
}

void pr::PriGraphicsScene::toggle_ruler()
{
    mp_ruler->setVisible(! mp_ruler->isVisible());
}

cm::Ruler *pr::PriGraphicsScene::ruler() const
{
    return mp_ruler;
}

// 此接口可后续扩充
void pr::PriGraphicsScene::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
    {
        // 获取当前选中的项
        QList<QGraphicsItem*> selectedItems = this->selectedItems();
        if(selectedItems.size() == 1)
        {
            // 从场景中移除选中的项
            sp::ShapeDrawGraphicsItem* shape_item = dynamic_cast<sp::ShapeDrawGraphicsItem*>(selectedItems[0]);
            if(shape_item)
            {
                QList<QGraphicsView *> graphics_views = views();
                Q_ASSERT(graphics_views.size() == 1);
                auto pri_graphics_view = dynamic_cast<pr::PriGraphicsView*>(graphics_views[0]);
                Q_ASSERT(pri_graphics_view);

                at::AttachTreeNode* tree_node = at::AttachTreeUtils::attach_tree_node_shape_item_in(shape_item, pri_graphics_view->current_primitive()->at_root());
                emit before_delete_tree_node(tree_node);
                //cmd::CommandManager::instance()->push(new cmd::RemoveCommand(tree_node));
                delete tree_node;
                emit after_delete_tree_node();
            }
        }
    }
    QGraphicsScene::keyPressEvent(event);
}
