#include "priCurveInputHBoxLayout.h"
#include "primitive.h"
#include "shapeDefines.h"
#include <PriShapeDialog.h>

#include <qcheckbox.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include "priUtils.h"

using namespace pr;

PriCurveTextEdit::PriCurveTextEdit(PriShapeDialog *parent)
    : QTextEdit(parent)
    , mp_dialog(parent)
{

}

pr::PriCurveTextEdit::~PriCurveTextEdit()
{

}

bool PriCurveTextEdit::is_valid() const
{
    return m_is_valid;
}

void pr::PriCurveTextEdit::focusOutEvent(QFocusEvent *event)
{
    QTextEdit::focusOutEvent(event);
    check_text();
}

void pr::PriCurveTextEdit::check_text()
{
    // 获取 QTextEdit 的内容
    QString text = this->toPlainText();

    const auto& text_edit_obj_name = objectName();

    bool is_valid = false;
    if(text_edit_obj_name.contains(SHAPE_CURVE_EQUATION))
    {
        is_valid = PriUtils::is_curve_equation_valid(text, mp_dialog->pri()->param_mgr());
    }
    else if(text_edit_obj_name.contains(SHAPE_CURVE_FIXED_POINT_SET))
    {
        is_valid = PriUtils::is_curve_fixed_point_set_valid(text, mp_dialog->pri()->param_mgr());
    }

    m_is_valid = is_valid;
}

pr::PriCurveInputHBoxLayout::PriCurveInputHBoxLayout(PriShapeDialog *parent, const QString &add_object_type, int count)
    : QHBoxLayout(parent),
      mp_dialog(parent),
      m_add_object_type(add_object_type),
      m_count(count)
{
    init();
}

PriCurveInputHBoxLayout::~PriCurveInputHBoxLayout()
{
    deinit();
}

const QString &pr::PriCurveInputHBoxLayout::add_object_type() const
{
    return m_add_object_type;
}

QString pr::PriCurveInputHBoxLayout::param_name() const
{
    return m_add_object_type + " " + QString::number(m_count);;
}

QString pr::PriCurveInputHBoxLayout::input_content() const
{
    // 获取 QTextEdit 的文本内容
    QString text = mp_input_widget->toPlainText();
    return text;
}

QString pr::PriCurveInputHBoxLayout::reverse_name() const
{
    return param_name() + " " + QString(SHAPE_CURVE_REVERSE);
}

QString pr::PriCurveInputHBoxLayout::reverse_value() const
{
    return mp_input_check_box->isChecked() ? SHAPE_CURVE_REVERSE_TURE : SHAPE_CURVE_REVERSE_FALSE;
}

void pr::PriCurveInputHBoxLayout::set_param_str(const QString &str)
{
    mp_input_widget->setText(str);
}

void pr::PriCurveInputHBoxLayout::set_reverse(bool reverse)
{
    mp_input_check_box->setChecked(reverse);
}

bool PriCurveInputHBoxLayout::is_valid() const
{
    return mp_input_widget->is_valid();
}

void PriCurveInputHBoxLayout::init()
{
    QString name_input_widget = param_name();
    mp_label = new QLabel(name_input_widget, mp_dialog);

    mp_input_widget = new PriCurveTextEdit(mp_dialog);
    mp_input_widget->setObjectName(name_input_widget);

    mp_input_check_box = new QCheckBox(mp_dialog);
    QString name_equation_input_check_box = reverse_name();
    mp_input_check_box->setObjectName(name_equation_input_check_box);

    mp_remove_input_button = new QPushButton(mp_dialog);
    mp_remove_input_button->setText("remove");

    addWidget(mp_label);
    addWidget(mp_input_widget);
    addWidget(mp_input_check_box);
    addWidget(mp_remove_input_button);

    auto succeed = connect(mp_remove_input_button, &QPushButton::clicked, this, &PriCurveInputHBoxLayout::remove_layout);
    Q_ASSERT(succeed);
}

void PriCurveInputHBoxLayout::deinit()
{
    // 去除信号和槽函数的链接
    auto succeed = disconnect(mp_remove_input_button, &QPushButton::clicked, this, &PriCurveInputHBoxLayout::remove_layout);
    Q_ASSERT(succeed);

    // 交由mp_dialog进行内存管理
    // SAFE_DELETE(mp_label);
    // SAFE_DELETE(mp_input_widget);
    // SAFE_DELETE(mp_input_check_box);
    // SAFE_DELETE(mp_remove_input_button);
}

void PriCurveInputHBoxLayout::remove_layout()
{
    emit remove_current_layout(this);
    mp_label->setVisible(false);
    mp_input_widget->setVisible(false);
    mp_input_check_box->setVisible(false);
    mp_remove_input_button->setVisible(false);
}
