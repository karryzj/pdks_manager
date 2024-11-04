#ifndef ADDCOMMAND_H
#define ADDCOMMAND_H

#include "commandBase.h"

namespace at
{
class AttachTreeNode;
class AttachTreeBaseNode;
}

namespace cmd
{

class CMD_PUBLIC AddCommand : public CommandBase
{
public:
    AddCommand(at::AttachTreeNode *tree_node, CommandBase* parent = nullptr);
    ~AddCommand() override;
    void undo() override;
    void redo() override;

private:
    at::AttachTreeNode *mp_tree_node;
    at::AttachTreeBaseNode *mp_parent_node;
    int m_parent_point_idx;
    int m_num;
};

}
#endif // ADDCOMMAND_H
