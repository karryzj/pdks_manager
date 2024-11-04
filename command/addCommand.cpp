#include "addCommand.h"
#include "attachTreeNode.h"
#include "attachTreeNodemgr.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include "shapedrawgraphicsitem.h"

namespace cmd
{
AddCommand::AddCommand(at::AttachTreeNode *tree_node, CommandBase* parent)
    : mp_tree_node(tree_node), mp_parent_node(tree_node->parent_node()),
      m_parent_point_idx(tree_node->get_parent_attach_point_idx()), m_num(0), CommandBase(parent)
{

}

AddCommand::~AddCommand()
{

}

void AddCommand::redo()
{
    if (mp_tree_node && (m_num > 0))
    {
        // auto point_id = mp_tree_node->get_parent_attach_point_idx();
        // auto parent = mp_tree_node->parent_node();
        if (mp_parent_node)
        {
            mp_tree_node->set_new_parent_node(mp_parent_node, m_parent_point_idx);
            mp_parent_node->add_child(mp_tree_node, m_parent_point_idx);
            mp_tree_node->update();
            mp_tree_node->tree_node_mgr()->update();
            mp_tree_node->view()->scene()->addItem(mp_tree_node->shape_item());
            m_num--;
        }
        // mp_tree_node->tree_node_mgr()->unregister_tree_node(mp_tree_node->id());
    }
}

void AddCommand::undo()
{
    if (mp_tree_node)
    {
        // auto parent = mp_tree_node->parent_node();
        if (mp_parent_node)
        {
            mp_parent_node->remove_child(mp_tree_node);
            mp_tree_node->set_new_parent_node(nullptr, 0);
            mp_parent_node->update();
            mp_parent_node->tree_node_mgr()->update();

            mp_parent_node->view()->scene()->removeItem(mp_tree_node->shape_item());
            m_num++;
        }
        // mp_tree_node->tree_node_mgr()->register_tree_node(mp_tree_node);
    }
}

}
