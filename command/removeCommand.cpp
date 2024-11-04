#include "removeCommand.h"

namespace cmd
{
RemoveCommand::RemoveCommand(at::AttachTreeNode *tree_node, CommandBase* parent)
    : mp_tree_node(tree_node), CommandBase(parent)
{

}

RemoveCommand::~RemoveCommand()
{

}

void RemoveCommand::undo()
{

}

void RemoveCommand::redo()
{
    if(false == m_has_init)
    {
        m_has_init = true;
    }



}

}
