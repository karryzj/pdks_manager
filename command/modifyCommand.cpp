#include "modifyCommand.h"

namespace cmd
{
ModifyCommand::ModifyCommand(at::AttachTreeNode *tree_node, CommandBase* parent)
    : mp_tree_node(tree_node), CommandBase(parent)
{

}

ModifyCommand::~ModifyCommand()
{

}

void ModifyCommand::undo()
{

}

void ModifyCommand::redo()
{

}

}
