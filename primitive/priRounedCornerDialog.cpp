#include "priRounedCornerDialog.h"
#include "common_defines.h"
#include "primitive.h"
#include "primitiveDefines.h"

#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>

#include "expression.h"
#include "configManager.h"

using namespace pr;

PriRounedCornerDialog::PriRounedCornerDialog(Primitive* pri)
    : QDialog()
    , mp_pri(pri)
{
    init();
}

PriRounedCornerDialog::~PriRounedCornerDialog()
{
    deinit();
}

void PriRounedCornerDialog::init()
{
    // 创建布局管理器
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QGridLayout *gridLayout = new QGridLayout();

    // 创建内径的 QLabel 和 QLineEdit
    QLabel *inner_corner_radius_label = new QLabel(CM_INNER_CORNER_RADIUS_KEY, this);
    QLineEdit *inner_corner_radius_line_edit = new QLineEdit(this);
    inner_corner_radius_line_edit->setObjectName(CM_INNER_CORNER_RADIUS_KEY);

    // 创建外径的 QLabel 和 QLineEdit
    QLabel *outer_corner_radius_label = new QLabel(CM_OUTER_CORNER_RADIUS_KEY, this);
    QLineEdit *outer_corner_radius_line_edit = new QLineEdit(this);
    outer_corner_radius_line_edit->setObjectName(CM_OUTER_CORNER_RADIUS_KEY);

    // 创建方向的 QLabel 和 QComboBox
    QLabel *direction_label = new QLabel(CM_OUTER_CORNER_DIRECTION_KEY, this);
    QComboBox *direction_combo_box = new QComboBox(this);
    direction_combo_box->setObjectName(CM_OUTER_CORNER_DIRECTION_KEY);
    direction_combo_box->addItem(CM_OUTER_CORNER_DIRECTION_TOP_RIGHT);
    direction_combo_box->addItem(CM_OUTER_CORNER_DIRECTION_TOP_LEFT);
    direction_combo_box->addItem(CM_OUTER_CORNER_DIRECTION_BOTTOM_LEFT);
    direction_combo_box->addItem(CM_OUTER_CORNER_DIRECTION_BOTTOM_RIGHT);

    // 将控件添加到布局中
    gridLayout->addWidget(inner_corner_radius_label, 0, 0);
    gridLayout->addWidget(inner_corner_radius_line_edit, 0, 1);
    gridLayout->addWidget(outer_corner_radius_label, 1, 0);
    gridLayout->addWidget(outer_corner_radius_line_edit, 1, 1);
    gridLayout->addWidget(direction_label, 2, 0);
    gridLayout->addWidget(direction_combo_box, 2, 1);

    // 创建确定和取消按钮
    QPushButton *ok_button = new QPushButton(PRI_OK, this);
    QPushButton *cancel_button = new QPushButton(PRI_CANCEL, this);

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    buttonLayout->addStretch(); // 添加一个弹性空间，使按钮靠右对齐
    buttonLayout->addWidget(ok_button);
    buttonLayout->addWidget(cancel_button);

    // 将按钮添加到布局中
    mainLayout->addLayout(gridLayout);
    mainLayout->addLayout(buttonLayout);

    // 连接按钮的信号到对话框的槽函数
    bool succeed = false;
    succeed = connect(ok_button, &QPushButton::clicked, this, &PriRounedCornerDialog::accept_modifies);
    Q_ASSERT(succeed);
    succeed = connect(cancel_button, &QPushButton::clicked, this, &QDialog::reject);
    Q_ASSERT(succeed);

    mp_inner_corner_radius_line_edit = inner_corner_radius_line_edit;
    mp_outer_corner_radius_line_edit = outer_corner_radius_line_edit;
    mp_direction_combo_box = direction_combo_box;

    auto current_inner_radius_value = cm::ConfigManager::instance()->query(CM_INNER_CORNER_RADIUS_KEY).toString();
    auto current_outer_radius_value = cm::ConfigManager::instance()->query(CM_OUTER_CORNER_RADIUS_KEY).toString();
    auto current_outer_direction_value = cm::ConfigManager::instance()->query(CM_OUTER_CORNER_DIRECTION_KEY).toString();

    mp_inner_corner_radius_line_edit->setText(current_inner_radius_value);
    mp_outer_corner_radius_line_edit->setText(current_outer_radius_value);
    mp_direction_combo_box->setCurrentText(current_outer_direction_value);
}

void PriRounedCornerDialog::deinit()
{

}

void PriRounedCornerDialog::accept_modifies()
{
    // 检查输入
    QString inner_corner_radius = mp_inner_corner_radius_line_edit->text();
    bool is_num = pm::Expression::is_num(inner_corner_radius);
    bool is_exp = pm::Expression::isExpression(mp_pri->param_mgr(), inner_corner_radius);
    if(false == is_num && false == is_exp)
    {
        // 弹窗警告输入有问题
        QString waring_info =  "The input of " + mp_inner_corner_radius_line_edit->objectName() + " is invalid, please input right value.";
        QMessageBox::warning(this, "Warning", waring_info);
        return;
    }

    QString outer_corner_radius = mp_outer_corner_radius_line_edit->text();
    is_num = pm::Expression::is_num(outer_corner_radius);
    is_exp = pm::Expression::isExpression(mp_pri->param_mgr(), outer_corner_radius);
    if(false == is_num && false == is_exp)
    {
        // 弹窗警告输入有问题
        QString waring_info =  "The input of " + mp_outer_corner_radius_line_edit->objectName() + " is invalid, please input right value.";
        QMessageBox::warning(this, "Warning", waring_info);
        return;
    }

    QString direction = mp_direction_combo_box->currentText();
    if(     direction != CM_OUTER_CORNER_DIRECTION_TOP_RIGHT &&
            direction != CM_OUTER_CORNER_DIRECTION_TOP_LEFT &&
            direction != CM_OUTER_CORNER_DIRECTION_BOTTOM_LEFT &&
            direction != CM_OUTER_CORNER_DIRECTION_BOTTOM_RIGHT)
    {
        // 弹窗警告输入有问题
        QString waring_info =  "The input of " + mp_direction_combo_box->objectName() + " is invalid, please input right value.";
        QMessageBox::warning(this, "Warning", waring_info);
        return;
    }

    cm::ConfigManager::instance()->setup_value(CM_INNER_CORNER_RADIUS_KEY, inner_corner_radius);
    cm::ConfigManager::instance()->setup_value(CM_OUTER_CORNER_RADIUS_KEY, outer_corner_radius);
    cm::ConfigManager::instance()->setup_value(CM_OUTER_CORNER_DIRECTION_KEY, CM_OUTER_CORNER_DIRECTION_DEFAULT_VALUE);

    QDialog::accept();
}

