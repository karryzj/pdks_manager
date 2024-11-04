#ifndef ROTATECOMMAND_H
#define ROTATECOMMAND_H

#include "modifyCommand.h"
#include "priRotateCfgDialog.h"

namespace cmd
{

class CMD_PUBLIC RotateCommand : public ModifyCommand
{
public:
    RotateCommand(at::AttachTreeNode *tree_node, RotateCfg rotate_cfg, RotateCfg old_rotate_cfg, ModifyCommand* parent = nullptr);
    virtual ~RotateCommand();
    void undo() override;
    void redo() override;

private:
    void node_rotate(at::AttachTreeNode * root_node, const RotateCfg &rotate_cfg);

    // at::AttachTreeNode *mp_tree_node;
    RotateCfg  m_rotate_cfg;
    RotateCfg  m_old_rotate_cfg;
    int m_num;

};
}

#endif // ROTATECOMMAND_H
