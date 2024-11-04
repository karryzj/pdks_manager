#include "priConvertShapeDialog.h"
#include "AttachTreeNode.h"
#include "attachTreeNodeMgr.h"
#include "primitiveDefines.h"
#include "shapeDefines.h"

#include <qboxlayout.h>
#include <qcombobox.h>
#include <qpushbutton.h>

using namespace pr;

priConvertShapeDialog::priConvertShapeDialog(at::AttachTreeNode *tree_node)
    : QDialog()
    , mp_tree_node(tree_node)
{
    init();
}

priConvertShapeDialog::~priConvertShapeDialog()
{

}

void priConvertShapeDialog::init()
{
    mp_convert_shape_combo_box = new QComboBox(this);  // 内存托管给PriMirrorDialog
    mp_convert_shape_combo_box->addItem(PRI_TREE_WIDGET_MENU_CONVERT_CIRCLE_ACTION_NAME);
    mp_convert_shape_combo_box->addItem(PRI_TREE_WIDGET_MENU_CONVERT_ELLIPSE_ACTION_NAME);
    mp_convert_shape_combo_box->addItem(PRI_TREE_WIDGET_MENU_CONVERT_QUARDANGLE_ACTION_NAME);
    mp_convert_shape_combo_box->addItem(PRI_TREE_WIDGET_MENU_CONVERT_SECTOR_ACTION_NAME);
    mp_convert_shape_combo_box->addItem(PRI_TREE_WIDGET_MENU_CONVERT_TRIANGLE_ACTION_NAME);

    m_convert_shape_lib[PRI_TREE_WIDGET_MENU_CONVERT_CIRCLE_ACTION_NAME] = SHAPE_CIRCLE;
    m_convert_shape_lib[PRI_TREE_WIDGET_MENU_CONVERT_ELLIPSE_ACTION_NAME] = SHAPE_ELLIPSE;
    m_convert_shape_lib[PRI_TREE_WIDGET_MENU_CONVERT_QUARDANGLE_ACTION_NAME] = SHAPE_QUADRANGLE;
    m_convert_shape_lib[PRI_TREE_WIDGET_MENU_CONVERT_SECTOR_ACTION_NAME] = SHAPE_SECTOR;
    m_convert_shape_lib[PRI_TREE_WIDGET_MENU_CONVERT_TRIANGLE_ACTION_NAME] = SHAPE_TRIANGLE;
    // 创建 OK 和 Cancel 按钮
    QPushButton *ok_button = new QPushButton(PRI_OK, this);
    QPushButton *cancel_button = new QPushButton(PRI_CANCEL, this);

    // 连接按钮的信号到对话框的槽
    connect(ok_button, &QPushButton::clicked, this, &priConvertShapeDialog::accept);
    connect(cancel_button, &QPushButton::clicked, this, &priConvertShapeDialog::reject);

    // 创建布局并添加控件
    QVBoxLayout *mainLayout = new QVBoxLayout(this);  // 内存托管给PriMirrorDialog
    mainLayout->addWidget(mp_convert_shape_combo_box);

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);  // 内存托管给PriMirrorDialog
    buttonLayout->addWidget(ok_button);
    buttonLayout->addWidget(cancel_button);

    mainLayout->addLayout(buttonLayout);
}

void priConvertShapeDialog::accept()
{
    QDialog::accept();

    auto item_text = mp_convert_shape_combo_box->currentText();

    mp_tree_node->set_shape_name(m_convert_shape_lib[item_text]);
    mp_tree_node->tree_node_mgr()->update();
    emit tree_widget_update();
}


