#include "rotateCommand.h"
#include "attachTreeNode.h"
#include "shapeBase.h"
#include "ParamDecl.h"
#include "shapeDefines.h"
#include "attachTreeNodemgr.h"

namespace cmd
{
RotateCommand::RotateCommand(at::AttachTreeNode *tree_node, RotateCfg rotate_cfg, RotateCfg old_rotate_cfg, ModifyCommand *parent)
    : m_rotate_cfg(rotate_cfg), m_old_rotate_cfg(old_rotate_cfg), m_num(0), ModifyCommand(tree_node, parent)
{

}

RotateCommand::~RotateCommand()
{

}

void RotateCommand::node_rotate(at::AttachTreeNode * root_node, const RotateCfg &rotate_cfg)
{
    auto func = [](at::AttachTreeNode * root_node, const RotateCfg &rotate_cfg)
    {
        QString rotate_degrees;
        rotate_degrees = rotate_cfg.degrees;
        if (!rotate_cfg.by_to)
        {
            auto pre_rotate = root_node->shape()->rotate();
            if (pre_rotate != 0)
            {
                rotate_degrees = rotate_cfg.degrees + "+" + QString::number(pre_rotate);
            }
        }

        auto params = root_node->params();
        for (auto &param : params)
        {
            if (param.key() == "rotate")
            {
                if (pm::Expression::is_expression(rotate_degrees))
                {
                    param.set_expression(rotate_degrees);
                }
                else
                {
                    param.set_expression(rotate_degrees);
                    param.set_value(rotate_degrees.toDouble());
                }
            }
        }
        root_node->set_update_children(false);
        root_node->set_params(params);
        root_node->set_update_children(true);
    };

    func(root_node, rotate_cfg);
    if (!rotate_cfg.child_self)
    {
        QVector<at::AttachTreeNode*> ready_to_update_nodes;
        const auto& mp = root_node->children();
        for(auto itor = mp.begin(); itor != mp.end(); itor++)
        {
            ready_to_update_nodes.append(itor.value());
        }

        while(!ready_to_update_nodes.empty())
        {
            QVector<at::AttachTreeNode*> next_nodes;
            for(auto node : ready_to_update_nodes)
            {
                node->set_update_children(false);
                func(node, rotate_cfg);
                node->set_update_children(true);

                const QMap<int, QVector<at::AttachTreeNode *> > & mp = node->children();
                for(auto itor = mp.begin(); itor != mp.end(); itor++)
                {
                    const auto& children = itor.value();
                    next_nodes.append(children);
                }
            }
            qSwap(next_nodes, ready_to_update_nodes);
        }
    }
    root_node->update();
}

void RotateCommand::undo()
{
    if (mp_tree_node)
    {
        auto old_rotate_cfg = m_old_rotate_cfg;
        old_rotate_cfg.degrees = m_old_rotate_cfg.degrees + "-" + m_rotate_cfg.degrees;
        // m_old_rotate_cfg.by_to = m_rotate_cfg.by_to;
        // m_old_rotate_cfg.child_self = m_rotate_cfg.child_self;
        node_rotate(mp_tree_node, old_rotate_cfg);
        mp_tree_node->tree_node_mgr()->update();
        m_num++;
    }
}

void RotateCommand::redo()
{
    if (mp_tree_node && (m_num > 0))
    {
        node_rotate(mp_tree_node, m_rotate_cfg);
        mp_tree_node->tree_node_mgr()->update();
        m_num--;
    }
}

}
