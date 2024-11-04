#ifndef MODIFYCOMMAND_H
#define MODIFYCOMMAND_H

#include "commandBase.h"

namespace at
{
class AttachTreeNode;
}

namespace cmd
{

class CMD_PUBLIC ModifyCommand : public CommandBase
{
public:
    ModifyCommand(at::AttachTreeNode *tree_node, CommandBase* parent = nullptr);
    virtual ~ModifyCommand();
    void undo() override;
    void redo() override;

protected:
    at::AttachTreeNode *mp_tree_node;
};

}
#endif // MODIFYCOMMAND_H
