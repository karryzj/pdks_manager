#ifndef REMOVECOMMAND_H
#define REMOVECOMMAND_H

#include "commandBase.h"

namespace at
{
class AttachTreeNode;
}

namespace cmd
{
class CMD_PUBLIC RemoveCommand : public CommandBase
{
public:
    RemoveCommand(at::AttachTreeNode *tree_node, CommandBase* parent = nullptr);
    ~RemoveCommand() override;
    void undo() override;
    void redo() override;

private:
    void init();

private:
    at::AttachTreeNode *mp_tree_node;
    bool m_has_init = false;


};

}
#endif // REMOVECOMMAND_H
