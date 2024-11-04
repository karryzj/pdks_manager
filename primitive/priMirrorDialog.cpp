#include "priMirrorDialog.h"
#include "AttachTreeNode.h"
#include "attachTreeNodeMgr.h"
#include "primitiveDefines.h"
#include "shapeDefines.h"

#include <qboxlayout.h>
#include <qcombobox.h>
#include <qpushbutton.h>

using namespace pr;

pr::PriMirrorDialog::PriMirrorDialog(at::AttachTreeNode *tree_node)
    : QDialog()
    , mp_tree_node(tree_node)
{
    init();
}

pr::PriMirrorDialog::~PriMirrorDialog()
{

}

void pr::PriMirrorDialog::init()
{
    mp_mirror_axis_combo_box = new QComboBox(this);  // 内存托管给PriMirrorDialog
    mp_mirror_axis_combo_box->setObjectName(PRI_TREE_WIDGET_MENU_MIRROR_AXIS);
    mp_mirror_axis_combo_box->addItem(PRI_TREE_WIDGET_MENU_MIRROR_X);
    mp_mirror_axis_combo_box->addItem(PRI_TREE_WIDGET_MENU_MIRROR_Y);

    // 创建 OK 和 Cancel 按钮
    QPushButton *ok_button = new QPushButton(PRI_OK, this);
    QPushButton *cancel_button = new QPushButton(PRI_CANCEL, this);

    // 连接按钮的信号到对话框的槽
    connect(ok_button, &QPushButton::clicked, this, &PriMirrorDialog::accept);
    connect(cancel_button, &QPushButton::clicked, this, &PriMirrorDialog::reject);

    // 创建布局并添加控件
    QVBoxLayout *mainLayout = new QVBoxLayout(this);  // 内存托管给PriMirrorDialog
    mainLayout->addWidget(mp_mirror_axis_combo_box);

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);  // 内存托管给PriMirrorDialog
    buttonLayout->addWidget(ok_button);
    buttonLayout->addWidget(cancel_button);

    mainLayout->addLayout(buttonLayout);
}

at::NodeDirection PriMirrorDialog::mirror_result_by_x(at::NodeDirection old_direction)
{
    auto new_direction = old_direction;
    if(old_direction == at::NodeDirection::NONE)
    {
        return new_direction;
    }

    if(old_direction == at::NodeDirection::TOP_LEFT)
    {
        new_direction = at::NodeDirection::BOTTOM_LEFT;
    }
    else if(old_direction == at::NodeDirection::BOTTOM_LEFT)
    {
        new_direction = at::NodeDirection::TOP_LEFT;
    }
    else if(old_direction == at::NodeDirection::BOTTOM_RIGHT)
    {
        new_direction = at::NodeDirection::TOP_RIGHT;
    }
    else if(old_direction == at::NodeDirection::TOP_RIGHT)
    {
        new_direction = at::NodeDirection::BOTTOM_RIGHT;
    }
    return new_direction;
}

at::NodeDirection PriMirrorDialog::mirror_result_by_y(at::NodeDirection old_direction)
{
    auto new_direction = old_direction;
    if(old_direction == at::NodeDirection::NONE)
    {
        return new_direction;
    }

    if(old_direction == at::NodeDirection::TOP_LEFT)
    {
        new_direction = at::NodeDirection::TOP_RIGHT;
    }
    else if(old_direction == at::NodeDirection::BOTTOM_LEFT)
    {
        new_direction = at::NodeDirection::BOTTOM_RIGHT;
    }
    else if(old_direction == at::NodeDirection::BOTTOM_RIGHT)
    {
        new_direction = at::NodeDirection::BOTTOM_LEFT;
    }
    else if(old_direction == at::NodeDirection::TOP_RIGHT)
    {
        new_direction = at::NodeDirection::TOP_LEFT;
    }
    return new_direction;
}

void PriMirrorDialog::accept()
{
    QDialog::accept();

    auto item_text = mp_mirror_axis_combo_box->currentText();

    bool mirror_by_x = false;
    if(PRI_TREE_WIDGET_MENU_MIRROR_X == item_text)
    {
        mirror_by_x = true;
    }
    else if(PRI_TREE_WIDGET_MENU_MIRROR_Y == item_text)
    {
        mirror_by_x = false;
    }
    else
    {
        Q_ASSERT(false);
    }

    std::function<void(at::AttachTreeBaseNode*)> proc = [&](at::AttachTreeBaseNode* base_tree_node)
    {
        auto tree_node = dynamic_cast<at::AttachTreeNode*>(base_tree_node);
        if(nullptr == tree_node)
        {
            return;
        }

        auto new_direction = tree_node->node_direction();
        if(mirror_by_x)
        {
            new_direction = mirror_result_by_x(new_direction);
        }
        else
        {
            new_direction = mirror_result_by_y(new_direction);
        }
        tree_node->set_update_children(false);
        tree_node->set_node_direction(new_direction);
        tree_node->set_update_children(true);

        auto params = tree_node->params();
        for(auto& param : params)
        {
            if(param.key() == SHAPE_ROTATE)
            {
                QString exp =  param.to_str();
                pm::Expression new_exp = pm::Expression(exp) * (-1);
                qreal new_value = new_exp.to_double(mp_tree_node->param_mgr());
                param.set_expression(new_exp.to_str());
                param.set_value(new_value);
                break;
            }
        }
        tree_node->set_params(params);
        return;
    };

    mp_tree_node->traversal_subtree(proc);
    mp_tree_node->set_update_children(true);
    mp_tree_node->update();
    mp_tree_node->tree_node_mgr()->update();
}
