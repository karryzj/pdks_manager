#include "priShapeDialog.h"
#include "Primitive.h"
#include "attachTreeNodeMgr.h"
#include "paramMgr.h"
#include "primitiveDefines.h"
#include <PriLayerInfoComboBox.h>

#include <ParamDecl.h>
#include <ParamUtils.h>
#include <ParamValidator.h>
#include <PriParamCheckBox.h>
#include <PriCurveInputHBoxLayout.h>
#include <PriUtils.h>
#include <QTextEdit>
#include <ShapeDecl.h>
#include <ShapeFactory.h>
#include <layInfo.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include "attachTreeUtils.h"
#include "attachTreeNode.h"
#include "qcheckbox.h"
#include "shapeDefines.h"

#include "shapeUtils.h"

using namespace pr;

PriShapeDialog::PriShapeDialog(const QString& shape_name,
                               bool add_type,
                               bool add_direction,
                               bool add_boolean_subtract_type,
                               bool add_layer,
                               ly::LayerWidget* layer_widget,
                               Primitive* pri)
    : QDialog()
    , m_shape_name(shape_name)
    , m_add_type(add_type)
    , m_add_direction(add_direction)
    , m_add_boolean_subtract_type(add_boolean_subtract_type)
    , m_add_layer(add_layer)
    , mp_layer_widget(layer_widget)
    , mp_pri(pri)
{
    init();
}

pr::PriShapeDialog::~PriShapeDialog()
{
    for(auto layout : m_layouts)
    {
        SAFE_DELETE(layout.first);
    }
    m_layouts.clear();
}

void PriShapeDialog::update_all()
{
    update_combo_box_boolean_subtract_type_info();
    update_combo_box_layer_info();
}

void pr::PriShapeDialog::init()
{
    // 创建UI面板
    this->setWindowTitle(QString(QObject::tr(PRI_WINDOW_TITLE_PREFIX)) + QString(" ") + m_shape_name);  // 设置面板名
    this->setFixedSize(PRI_DIALOG_WIDTH_SIZE, PRI_DIALOG_HEIGHT_SIZE);

    QWidget *content_widget = new QWidget(this);
    QVBoxLayout* context_layout = new QVBoxLayout(
        content_widget);  // 用于将子控件垂直排列在一个父控件中。它会自动管理子控件的位置和大小，使它们在父控件中按从上到下的顺序排列。
    m_context_layout = context_layout;
    // 滚动条
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(content_widget);
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addWidget(scrollArea);

    // 类型
    if (m_add_type)
    {
        QHBoxLayout* type_layout = new QHBoxLayout(this);
        QLabel* type_label = new QLabel(QObject::tr(PRI_TYPE));       // 类型标签
        QComboBox* type_combo_box = new QComboBox(this);              // 创建下拉列表
        type_combo_box->setObjectName(QObject::tr(PRI_TYPE_TYPEOPTION));
        type_combo_box->addItems({QObject::tr(PRI_TYPE_LOCATION),
                                  QObject::tr(PRI_TYPE_ADD),
                                  QObject::tr(PRI_TYPE_DELETE)});      // 增加定位，增加，删除三个选项。
        type_layout->addWidget(type_label);
        type_layout->addWidget(type_combo_box);
        context_layout->addLayout(type_layout);
    }

    // 放置方向
    if (m_add_direction)
    {
        QHBoxLayout* direction_layout = new QHBoxLayout(this);
        QLabel* direction_label = new QLabel(QObject::tr(PRI_DIRECTION_TYPEOPTION));
        QComboBox* direction_combo_box = new QComboBox(this);
        direction_combo_box->setObjectName(QObject::tr(PRI_DIRECTION_TYPEOPTION));
        direction_combo_box->addItems({QObject::tr(PRI_DIRECTION_TOP_LEFT),
                                       QObject::tr(PRI_DIRECTION_TOP_RIGHT),
                                       QObject::tr(PRI_DIRECTION_BOTTOM_LEFT),
                                       QObject::tr(PRI_DIRECTION_BOTTOM_RIGHT)});
        direction_layout->addWidget(direction_label);
        direction_layout->addWidget(direction_combo_box);
        context_layout->addLayout(direction_layout);
    }

    // 布尔减类型
    if (m_add_boolean_subtract_type)
    {
        QHBoxLayout* boolean_subtract_layout = new QHBoxLayout(this);
        QLabel* boolean_subtract_label = new QLabel(QObject::tr(PRI_BOOLEAN_SUBTRACT_TYPE));
        QComboBox* boolean_subtract_combo_box = new QComboBox(this);
        boolean_subtract_combo_box->setObjectName(QObject::tr(PRI_BOOLEAN_SUBTRACT_TYPE_TYPE_OPTION));
        boolean_subtract_layout->addWidget(boolean_subtract_label);
        boolean_subtract_layout->addWidget(boolean_subtract_combo_box);
        context_layout->addLayout(boolean_subtract_layout);
    }

    // 图层
    if (m_add_layer)
    {
        QHBoxLayout* layer_layout = new QHBoxLayout(this);
        QLabel* layer_label = new QLabel(QObject::tr(PRI_LAYER));
        PriLayerInfoComboBox* layer_combo_box = new PriLayerInfoComboBox(mp_layer_widget, this);
        layer_combo_box->setObjectName(tr(
                                           PRI_LAYER_TYPEOPTION));     // HINT@leixunyong。这里需要手动在业务层添加可供选择的图层选项。
        layer_layout->addWidget(layer_label);
        layer_layout->addWidget(layer_combo_box);
        context_layout->addLayout(layer_layout);
    }

    // 参数列表
    // 1.特殊参数(不在参数列表中)
    {
        // TODO@leixunyong
    }
    // 2.通用参数
    {
        sp::ShapeFactory* shapeIns = sp::ShapeFactory::instance();
        QMap<QString, sp::ShapeDecl*> shapeDecl = shapeIns->get_shapes_decl();

        auto itor = shapeDecl.find(m_shape_name);
        Q_ASSERT(itor != shapeDecl.end());

        auto& shape_decl = itor.value();

        // 获取形状名称
        QString current_shape_name = shape_decl->name();
        setObjectName(current_shape_name);
        // 获取形状资源路径
        QString shape_icon_path = shape_decl->icon();

        auto param_list = shape_decl->param_list();
        for (auto idx = 0; idx < param_list.size(); idx++)
        {
            const QString& param_name = param_list[idx].first;
            const QVariant& var = param_list[idx].second;
            if(var.type() == QVariant::Double)  // qreal
            {
                qreal param_default_value = var.toReal();
                //增加参数的label
                QLabel * label = new QLabel(param_name);

                //增加参数的输入框
                QLineEdit * line_edit = new QLineEdit(this);
                bool succeed = connect(line_edit, &QLineEdit::editingFinished, this, &PriShapeDialog::update_params);
                Q_ASSERT(succeed);
                {
                    // 创建自定义验证器
                    pm::ParamValidator *validator = new pm::ParamValidator(line_edit);
                    // 设置验证器
                    line_edit->setValidator(validator);
                }
                line_edit->setObjectName(param_name);
                line_edit->setText(QString::number(param_default_value));

                //增加一个LayerLayout，将两个label和输入框放在一行
                QHBoxLayout * param_layout = new QHBoxLayout(this);

                //将label和输入框添加到布局中
                param_layout->addWidget(label);
                param_layout->addWidget(line_edit);
                if(m_shape_name == SHAPE_POLYGEN && param_name != SHAPE_ROTATE)
                {
                    PriParamCheckBox* param_check_box = new PriParamCheckBox(param_name, this);
                    auto succeed = connect(param_check_box, &PriParamCheckBox::change_extracted_param_status, this, &PriShapeDialog::update_polygen_point_param_display);
                    Q_ASSERT(succeed);
                    param_layout->addWidget(param_check_box);
                }
                //将所有行添加到主布局
                context_layout->addLayout(param_layout);
            }
            else
            {
                Q_ASSERT(false);   // HINT@leixunyong。走到这里来的时候说明需要处理一些新增类型。
            }
        }
    }

    // 3.特殊参数
    {
        if(m_shape_name == SHAPE_CURVE)   // 额外增加2个按钮，分别实现添加方程和添加固定点集的功能
        {
            //增加一个LayerLayout，将两个label和输入框放在一行
            QHBoxLayout * add_button_layout = new QHBoxLayout(this);

            QPushButton* add_equation_button = new QPushButton(this);
            add_equation_button->setText("add equation");
            add_button_layout->addWidget(add_equation_button);
            connect(add_equation_button, & QPushButton::clicked, this, & PriShapeDialog::add_equation_input_widget);

            QPushButton* add_point_button = new QPushButton(this);
            add_point_button->setText("add fixed point set");
            add_button_layout->addWidget(add_point_button);
            connect(add_point_button, & QPushButton::clicked, this, & PriShapeDialog::add_fixed_point_set_input_widget);
            //将所有行添加到主布局
            context_layout->addLayout(add_button_layout);
        }
    }

    // 分隔线
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    context_layout->addWidget(line);

    // 右下角增加 确定和取消按钮
    QHBoxLayout* button_layout = new QHBoxLayout(this);
    QPushButton* ok_button = new QPushButton(QObject::tr(PRI_OK));
    connect(ok_button, & QPushButton::clicked, this, & PriShapeDialog::accept_modifies);

    ok_button->setObjectName(QObject::tr(PRI_OK));
    QPushButton* cancel_button = new QPushButton(QObject::tr(PRI_CANCEL));
    connect(cancel_button, & QPushButton::clicked, this, & QDialog::close);
    cancel_button->setObjectName(QObject::tr(PRI_CANCEL));
    button_layout->addWidget(ok_button);
    button_layout->addWidget(cancel_button);

    main_layout->addLayout(button_layout);

    setup_connect();

    update_all();
}

pr::Primitive *pr::PriShapeDialog::pri() const
{
    return mp_pri;
}

at::NodeType pr::PriShapeDialog::shape_node_type() const
{
    // 读取type
    at::NodeType node_type = at::NodeType::NONE;
    auto type_option_combo_box = findChild<QComboBox*>(tr(PRI_TYPE_TYPEOPTION));
    if (type_option_combo_box)
    {
        if (tr(PRI_TYPE_LOCATION) == type_option_combo_box->currentText())
        {
            node_type = at::NodeType::LOCATION;
        }
        else if (tr(PRI_TYPE_ADD) == type_option_combo_box->currentText())
        {
            node_type = at::NodeType::ADD;
        }
        else if (tr(PRI_TYPE_DELETE) == type_option_combo_box->currentText())
        {
            node_type = at::NodeType::DELETE;
        }
    }
    return node_type;
}

at::NodeDirection pr::PriShapeDialog::shape_node_direction() const
{
    // 读取方向
    at::NodeDirection node_direction = at::NodeDirection::NONE;
    auto direction_option_combo_box = findChild<QComboBox*>(tr(PRI_DIRECTION_TYPEOPTION));
    if (direction_option_combo_box)
    {
        if (tr(PRI_DIRECTION_TOP_LEFT) == direction_option_combo_box->currentText())
        {
            node_direction = at::NodeDirection::TOP_LEFT;
        }
        else if (tr(PRI_DIRECTION_TOP_RIGHT) == direction_option_combo_box->currentText())
        {
            node_direction = at::NodeDirection::TOP_RIGHT;
        }
        else if (tr(PRI_DIRECTION_BOTTOM_LEFT) == direction_option_combo_box->currentText())
        {
            node_direction = at::NodeDirection::BOTTOM_LEFT;
        }
        else if (tr(PRI_DIRECTION_BOTTOM_RIGHT) == direction_option_combo_box->currentText())
        {
            node_direction = at::NodeDirection::BOTTOM_RIGHT;
        }
    }
    return node_direction;
}

at::NodeBooleanSubtractType pr::PriShapeDialog::shape_node_boolean_subtract_type() const
{
    // 读取布尔减类型
    at::NodeBooleanSubtractType node_boolean_subtract_type = at::NodeBooleanSubtractType::NONE;
    auto boolean_subtract_type_option_combo_box = findChild<QComboBox*>(tr(PRI_BOOLEAN_SUBTRACT_TYPE_TYPE_OPTION));
    if (boolean_subtract_type_option_combo_box)
    {
        if (tr(PRI_BOOLEAN_SUBTRACT_NONE) == boolean_subtract_type_option_combo_box->currentText())
        {
            node_boolean_subtract_type = at::NodeBooleanSubtractType::NONE;
        }
        else if (tr(PRI_BOOLEAN_SUBTRACT_ONLY_PARENT) == boolean_subtract_type_option_combo_box->currentText())
        {
            node_boolean_subtract_type = at::NodeBooleanSubtractType::ONLY_WITH_PARENT_NODE;
        }
        else if (tr(PRI_BOOLEAN_SUBTRACT_ONLY_PARENT_AND_BROTHER) == boolean_subtract_type_option_combo_box->currentText())
        {
            node_boolean_subtract_type = at::NodeBooleanSubtractType::WITH_PARENT_AND_BROTHER_NODE;
        }
        else if (tr(PRI_BOOLEAN_SUBTRACT_PARENT_AND_CHILD) == boolean_subtract_type_option_combo_box->currentText())
        {
            node_boolean_subtract_type = at::NodeBooleanSubtractType::WITH_PARENT_AND_CHILD_NODE;
        }
    }
    return node_boolean_subtract_type;
}

QString pr::PriShapeDialog::shape_layer_info_name() const
{
    // 读取图层 TODO@leixunyong
    QString layer_info_name = {};
    auto layer_option_combo_box = findChild<QComboBox*>(tr(PRI_LAYER_TYPEOPTION));
    if (layer_option_combo_box)
    {
        layer_info_name = layer_option_combo_box->currentText();
    }
    return layer_info_name;
}

const QVector<pm::ParamDecl>& pr::PriShapeDialog::shape_param_list() const
{
    return m_params;
}

QVariant pr::PriShapeDialog::get_special_param(const QString &param_name) const
{
    if(param_name == SHAPE_POLYGEN)
    {
        auto shape_is_extract_param_check_box = findChild<QCheckBox*>(tr(SHAPE_POLYGEN));
        Q_ASSERT(shape_is_extract_param_check_box);

        bool is_extract_param = shape_is_extract_param_check_box->isChecked();
        return is_extract_param;
    }
    else if(false)
    {
        // ...
    }


    Q_ASSERT(false);  // HINT@leixunyong。走到这里来的时候说明有shape类型匹配的问题。
    return QVariant{};
}

void PriShapeDialog::init_ui_in_create_shape()
{
    mp_editing_tree_node = nullptr;
    // 类型
    set_shape_node_type(at::NodeType::LOCATION);
    set_shape_node_direction(at::NodeDirection::TOP_LEFT);
    set_shape_node_boolean_subtract_type(at::NodeBooleanSubtractType::NONE);
    if(SHAPE_POLYGEN == m_shape_name)
    {
        set_shape_node_direction(at::NodeDirection::TOP_RIGHT);
    }
    set_shape_layer_info_name(findChild<PriLayerInfoComboBox*>(PRI_LAYER_TYPEOPTION)->default_layer_info());

    sp::ShapeFactory* shapeIns = sp::ShapeFactory::instance();
    QMap<QString, sp::ShapeDecl*> shapeDecl = shapeIns->get_shapes_decl();

    auto itor = shapeDecl.find(m_shape_name);
    Q_ASSERT(itor != shapeDecl.end());

    const auto& shape_decl = itor.value();
    const QVector<QPair<QString, QVariant>>& params = shape_decl->param_list();

    set_shape_param_list(params);
}

void pr::PriShapeDialog::init_ui_in_edit_shape(sp::ShapeDrawGraphicsItem *shape_item)
{
    mp_editing_tree_node = at::AttachTreeUtils::attach_tree_node_shape_item_in(shape_item, mp_pri->at_root());
    Q_ASSERT(mp_editing_tree_node);
    auto node_type = mp_editing_tree_node->node_type();
    auto node_direction = mp_editing_tree_node->node_direction();
    auto node_boolean_subtract_type = mp_editing_tree_node->node_boolean_subtract_type();
    auto layer_info = mp_editing_tree_node->layer_info();
    auto params = mp_editing_tree_node->params();

    set_shape_node_type(node_type);
    set_shape_node_direction(node_direction);
    set_shape_node_boolean_subtract_type(node_boolean_subtract_type);
    set_shape_layer_info_name(layer_info);

    QVector<QPair<QString, QVariant>> param_list;
    for(auto param : params)
    {
        QPair<QString, QVariant> new_pair {param.key(), param.expression()};
        param_list.push_back(new_pair);
    }
    set_shape_param_list(param_list);

    // 设置全部checkbox
    QList<PriParamCheckBox*> param_check_boxs = findChildren<PriParamCheckBox*>();
    for(auto check_box : param_check_boxs)
    {
        QString param_name = check_box->param_name();
        QLineEdit* line_edit = findChild<QLineEdit*>(param_name);
        if(nullptr == line_edit)
        {
            continue;
        }
        QString param = line_edit->text();
        if(!pm::Expression::is_num(param))
        {
            check_box->setEnabled(false);
        }
    }
    // HINT@leixunyong。针对曲线的特化
    if(m_shape_name == SHAPE_CURVE)
    {
        process_ui_for_curve_edit(param_list);
    }
}

at::AttachTreeNode *pr::PriShapeDialog::editing_tree_node() const
{
    return mp_editing_tree_node;
}

void pr::PriShapeDialog::set_shape_node_type(at::NodeType node_type)
{
    auto type_option_combo_box = findChild<QComboBox*>(tr(PRI_TYPE_TYPEOPTION));
    if (type_option_combo_box)
    {
        if(node_type == at::NodeType::LOCATION)
        {
            type_option_combo_box->setCurrentText(tr(PRI_TYPE_LOCATION));
        }
        else if(node_type == at::NodeType::ADD)
        {
            type_option_combo_box->setCurrentText(tr(PRI_TYPE_ADD));
        }
        else if(node_type == at::NodeType::DELETE)
        {
            type_option_combo_box->setCurrentText(tr(PRI_TYPE_DELETE));
        }
    }

    update_all();
}

void pr::PriShapeDialog::set_shape_node_direction(at::NodeDirection node_direction)
{
    // 读取方向
    auto direction_option_combo_box = findChild<QComboBox*>(tr(PRI_DIRECTION_TYPEOPTION));
    if (direction_option_combo_box)
    {
        if(node_direction == at::NodeDirection::TOP_LEFT)
        {
            direction_option_combo_box->setCurrentText(tr(PRI_DIRECTION_TOP_LEFT));
        }
        else if(node_direction == at::NodeDirection::TOP_RIGHT)
        {
            direction_option_combo_box->setCurrentText(tr(PRI_DIRECTION_TOP_RIGHT));
        }
        else if (node_direction == at::NodeDirection::BOTTOM_LEFT)
        {
            direction_option_combo_box->setCurrentText(tr(PRI_DIRECTION_BOTTOM_LEFT));
        }
        else if  (node_direction == at::NodeDirection::BOTTOM_RIGHT)
        {
            direction_option_combo_box->setCurrentText(tr(PRI_DIRECTION_BOTTOM_RIGHT));
        }
    }
}

void pr::PriShapeDialog::set_shape_node_boolean_subtract_type(at::NodeBooleanSubtractType boolean_subtract_type)
{
    // 读取方向
    auto boolean_subtract_type_option_combo_box = findChild<QComboBox*>(tr(PRI_BOOLEAN_SUBTRACT_TYPE_TYPE_OPTION));
    if (boolean_subtract_type_option_combo_box)
    {
        if(boolean_subtract_type == at::NodeBooleanSubtractType::NONE)
        {
            boolean_subtract_type_option_combo_box->setCurrentText(tr(PRI_BOOLEAN_SUBTRACT_NONE));
        }
        else if(boolean_subtract_type == at::NodeBooleanSubtractType::ONLY_WITH_PARENT_NODE)
        {
            boolean_subtract_type_option_combo_box->setCurrentText(tr(PRI_BOOLEAN_SUBTRACT_ONLY_PARENT));
        }
        else if (boolean_subtract_type == at::NodeBooleanSubtractType::WITH_PARENT_AND_BROTHER_NODE)
        {
            boolean_subtract_type_option_combo_box->setCurrentText(tr(PRI_BOOLEAN_SUBTRACT_ONLY_PARENT_AND_BROTHER));
        }
        else if  (boolean_subtract_type == at::NodeBooleanSubtractType::WITH_PARENT_AND_CHILD_NODE)
        {
            boolean_subtract_type_option_combo_box->setCurrentText(tr(PRI_BOOLEAN_SUBTRACT_PARENT_AND_CHILD));
        }
    }
}

void pr::PriShapeDialog::set_shape_layer_info_name(const ly::LayerInfo *layer_info)
{
    auto combo_box_layer_info = findChild<PriLayerInfoComboBox*>(PRI_LAYER_TYPEOPTION);
    Q_ASSERT(combo_box_layer_info);
    if(nullptr == layer_info)
    {
        return;
    }

    auto layer_info_name = layer_info->layer_name();

    combo_box_layer_info->setCurrentText(layer_info_name);
}

void pr::PriShapeDialog::set_shape_param_list(const QVector<QPair<QString, QVariant>>& param_list)
{
    QVector<pm::ParamDecl> params;
    for (auto idx = 0; idx < param_list.size(); idx++)
    {
        const QString& param_name = param_list[idx].first;
        const QVariant& param_default_value = param_list[idx].second;
        pm::ParamDecl param(param_name, param_default_value);
        params.append(param);
        if(param_default_value.type() == QVariant::Double || param_default_value.type() == QVariant::String)
        {
            QLineEdit* line_edit = findChild<QLineEdit*>(param_name);

            QString text_str = param_default_value.toString();
            if(!line_edit)
            {
                continue;
            }
            text_str = pm::ParamUtils::str_to_3_pre_str(text_str);
            line_edit->setText(text_str);
            if(SHAPE_POLYGEN == m_shape_name)
            {
                if(line_edit->objectName() == SHAPE_ROTATE)
                {
                    continue;
                }

                bool is_extracted_param = !pm::Expression::is_num(text_str);
                auto check_box = findChild<QCheckBox*>(param_name);
                auto line_edit = findChild<QLineEdit*>(param_name);
                if(check_box && line_edit)
                {
                    check_box->setChecked(is_extracted_param);
                    line_edit->setEnabled(!is_extracted_param);
                }
            }
        }
        else
        {
            Q_ASSERT(false);  // HINT@leixunyong。走到这里来说明类型有问题。
        }
    }

    if(nullptr == mp_editing_tree_node)
    {
        qSwap(m_params, params);
    }
    else
    {
        m_params = mp_editing_tree_node->params();
    }
}

void pr::PriShapeDialog::update_line_edits_by_params()
{
    int node_id = (mp_editing_tree_node == nullptr ?  mp_pri->tree_node_mgr()->id_number() + 1 : mp_editing_tree_node->id());

    QRegularExpression regex("([xy])(\\d+)");

    if(m_params.isEmpty())
    {
        return;
    }

    QMap<QString, qreal> param_key_to_value;
    for(const auto& param : m_params)
    {
        // 记录值
        const auto& param_key = param.key();
        QRegularExpressionMatch match = regex.match(param_key);
        if(!match.hasMatch())
        {
            continue;
        }
        qreal param_value = param.to_double(mp_pri->param_mgr());
        param_key_to_value[param_key] = param_value;
        // 获得对应的自动提参行为和参数值
        PriParamCheckBox* current_check_box = findChild<PriParamCheckBox*>(param_key);
        QLineEdit* current_line_edit = findChild<QLineEdit*>(param_key);
        Q_ASSERT(nullptr != current_check_box && nullptr != current_line_edit);
        if(!current_check_box->isChecked())
        {
            QString number_str = QString::number(param_key_to_value[param_key]);
            number_str = pm::ParamUtils::str_to_3_pre_str(number_str);
            current_line_edit->setText(number_str);
            current_line_edit->setEnabled(true);
        }
        else
        {
            QString delat_value = "N" + QString::number(node_id) + "d" + param_key;

            QString character = match.captured(1);
            QString number_str = match.captured(2);
            int number = number_str.toInt();
            if(number == 1)
            {
                current_line_edit->setText(delat_value);
            }
            else
            {
                auto last_param_key = character + QString::number(number - 1);
                auto last_param_value = param_key_to_value[last_param_key];
                QLineEdit * last_line_edit = findChild<QLineEdit * >(last_param_key);
                PriParamCheckBox * last_check_box = findChild<PriParamCheckBox * >(last_param_key);
                if(!last_check_box->isChecked())  // 情况1：上一个是数值，无论如何都要提参
                {
                    current_line_edit->setText(last_line_edit->text() + "+" + delat_value);
                }
                else
                {
                    if(last_param_value != param_key_to_value[param_key]) // 情况1：上一个是表达式，但是和当前的值不一样，也需要提参
                    {
                        current_line_edit->setText(last_line_edit->text() + "+" + delat_value);
                    }
                    else
                    {
                        current_line_edit->setText(last_line_edit->text());
                    }
                }
            }
            current_line_edit->setEnabled(false);
        }
    }
}

bool pr::PriShapeDialog::equation_is_vaild(const QString &equation) const
{
    // TODO@leixunyong.
    return true;
}

bool pr::PriShapeDialog::fixed_point_set_is_valid(const QString &fixed_point_set) const
{
    QRegularExpression pattern("^[^\\n]+(\\n[^\\n]+)*$");
    // 使用正则表达式匹配字符串
    if(!pattern.match(fixed_point_set).hasMatch())
    {
        return false;
    }

    auto sub_strs = sp::ShapeUtils::split_string(fixed_point_set, "\n");
    // 定义正则表达式模式
    pattern.setPattern(("^[^,]+,[^,]+$"));
    for(auto sub_str : sub_strs)
    {
        // 使用正则表达式匹配字符串
        if(! pattern.match(sub_str).hasMatch())
        {
            return false;
        }
    }

    return true;
}

void pr::PriShapeDialog::process_ui_for_curve_edit(const QVector<QPair<QString, QVariant> > &param_pairs)
{
    // 先把和方程和点集的参数过滤出来
    QVector<QPair<QString, QVariant> > vaild_param_pairs;
    for(auto param : param_pairs)
    {
        const auto& key = param.first;
        if(key.contains(SHAPE_CURVE_EQUATION) ||  key.contains(SHAPE_CURVE_FIXED_POINT_SET))
        {
            vaild_param_pairs.append(param);
        }
    }

    int add_equation_count = 0;
    int add_fixed_point_count = 0;

    auto get_str_number = [](const QString& str)
    {
        QRegularExpression re("\\b(\\d+)$");
        QRegularExpressionMatch match = re.match(str);

        if (match.hasMatch())
        {
            return match.captured(1).toInt();
        }

        Q_ASSERT(false);
        return -1;
    };

    for(auto idx = 0; idx < vaild_param_pairs.size() - 1; idx += 2)
    {
        auto str_param = vaild_param_pairs[idx];

        PriCurveInputHBoxLayout * layout = nullptr;
        if(str_param.first.contains(SHAPE_CURVE_EQUATION))
        {
            add_equation_count = get_str_number(str_param.first);
            layout = new PriCurveInputHBoxLayout(this, QString(SHAPE_CURVE_EQUATION), add_equation_count);
            auto succeed = connect(layout, &PriCurveInputHBoxLayout::remove_current_layout, this, &pr::PriShapeDialog::remove_curve_input_layout);
            Q_ASSERT(succeed);
            //将所有行添加到主布局
            m_context_layout->addLayout(layout);
            m_layouts.append(QPair<PriCurveInputHBoxLayout*, bool>(layout, true));
        }
        else if(str_param.first.contains(SHAPE_CURVE_FIXED_POINT_SET))
        {
            add_fixed_point_count = get_str_number(str_param.first);
            layout = new PriCurveInputHBoxLayout(this, QString(SHAPE_CURVE_FIXED_POINT_SET), add_fixed_point_count);
            auto succeed = connect(layout, &PriCurveInputHBoxLayout::remove_current_layout, this, &pr::PriShapeDialog::remove_curve_input_layout);
            Q_ASSERT(succeed);
            //将所有行添加到主布局
            m_context_layout->addLayout(layout);
            m_layouts.append(QPair<PriCurveInputHBoxLayout*, bool>(layout, true));
        }
        else
        {
            Q_ASSERT(false);
        }
        layout->set_param_str(str_param.second.toString());

        auto reverse_param = vaild_param_pairs[idx + 1];
        Q_ASSERT(reverse_param.first.contains(SHAPE_CURVE_REVERSE));
        bool reverse = false;
        if(reverse_param.second == SHAPE_CURVE_REVERSE_TURE)
        {
            reverse = true;
        }
        else if(reverse_param.second == SHAPE_CURVE_REVERSE_FALSE)
        {
            reverse = false;
        }
        else
        {
            Q_ASSERT(false);
        }
        layout->set_reverse(reverse);
    }
    m_add_equation_count = add_equation_count;
    m_add_fixed_point_count = add_fixed_point_count;
}

void PriShapeDialog::setup_connect()
{
    // 联动修改
    auto combo_box_type = findChild<QComboBox*>(PRI_TYPE_TYPEOPTION);
    Q_ASSERT(combo_box_type);

    auto combo_box_boolean_subtract_type_option_info = findChild<PriLayerInfoComboBox*>(PRI_BOOLEAN_SUBTRACT_TYPE_TYPE_OPTION);
    Q_ASSERT(combo_box_boolean_subtract_type_option_info);
    connect(combo_box_type, QOverload<int>::of( & QComboBox::currentIndexChanged),
            this, & PriShapeDialog::update_combo_box_layer_info);

    auto combo_box_layer_info = findChild<PriLayerInfoComboBox*>(PRI_LAYER_TYPEOPTION);
    Q_ASSERT(combo_box_layer_info);
    connect(combo_box_type, QOverload<int>::of( & QComboBox::currentIndexChanged),
            this, & PriShapeDialog::update_combo_box_boolean_subtract_type_info);
}

void PriShapeDialog::update_combo_box_layer_info()
{
    auto combo_box_type = findChild<QComboBox*>(PRI_TYPE_TYPEOPTION);
    Q_ASSERT(combo_box_type);

    auto combo_box_layer_info = findChild<PriLayerInfoComboBox*>(PRI_LAYER_TYPEOPTION);
    Q_ASSERT(combo_box_layer_info);

    combo_box_layer_info->clear();
    if (combo_box_type->currentText() == tr(PRI_TYPE_LOCATION) || combo_box_type->currentText() == tr(PRI_TYPE_DELETE))
    {
        combo_box_layer_info->display_only_location_layer_info_item();
    }
    else if (combo_box_type->currentText() == tr(PRI_TYPE_ADD) )
    {
        combo_box_layer_info->display_only_vaild_layer_info_item();
    }
}

void pr::PriShapeDialog::update_combo_box_boolean_subtract_type_info()
{
    auto combo_box_type = findChild<QComboBox*>(PRI_TYPE_TYPEOPTION);
    Q_ASSERT(combo_box_type);

    auto combo_box_boolean_subtract_type_option_info = findChild<PriLayerInfoComboBox*>(PRI_BOOLEAN_SUBTRACT_TYPE_TYPE_OPTION);
    Q_ASSERT(combo_box_boolean_subtract_type_option_info);

    if (combo_box_type->currentText() == tr(PRI_TYPE_LOCATION) || combo_box_type->currentText() == tr(PRI_TYPE_ADD) )
    {
        combo_box_boolean_subtract_type_option_info->clear();
        combo_box_boolean_subtract_type_option_info->addItems( QStringList
        {
            QObject::tr(PRI_BOOLEAN_SUBTRACT_NONE)
        });
        combo_box_boolean_subtract_type_option_info->setEnabled(false);
    }
    else if(combo_box_type->currentText() == tr(PRI_TYPE_DELETE))
    {
        combo_box_boolean_subtract_type_option_info->clear();
        combo_box_boolean_subtract_type_option_info->addItems( QStringList
        {
            QObject::tr(PRI_BOOLEAN_SUBTRACT_ONLY_PARENT),
            QObject::tr(PRI_BOOLEAN_SUBTRACT_ONLY_PARENT_AND_BROTHER),
            QObject::tr(PRI_BOOLEAN_SUBTRACT_PARENT_AND_CHILD)
        });
        combo_box_boolean_subtract_type_option_info->setEnabled(true);
    }
}

void pr::PriShapeDialog::update_params()
{
    for(auto& param : m_params)
    {
        const auto& param_key = param.key();
        QLineEdit* param_line_edit = findChild<QLineEdit*>(param_key);
        if(nullptr == param_line_edit)
        {
            continue;
        }
        QString line_edit_value = param_line_edit->text();
        param.set_expression(line_edit_value);
        if(pm::Expression::is_num(line_edit_value))
        {
            param.set_value(line_edit_value.toDouble());
        }
    }
}

void pr::PriShapeDialog::update_polygen_point_param_display(bool checked, const QString& param_name)
{
    PriParamCheckBox* current_check_box = findChild<PriParamCheckBox*>(param_name);
    if(nullptr == current_check_box)
    {
        return;
    }
    current_check_box->setChecked(checked);

    update_line_edits_by_params();
}

void pr::PriShapeDialog::accept_modifies()
{
    // 将QLineEdit上的值应用于参数
    auto params_cache = m_params;
    for(auto& param : m_params)
    {
        const auto& param_key = param.key();
        QLineEdit* param_line_edit = findChild<QLineEdit*>(param_key);
        if(nullptr == param_line_edit)
        {
            continue;
        }
        QString line_edit_value = param_line_edit->text();
        param.set_expression(param_line_edit->text()); // 这里分为2种情况：1.是数字。直接设置即可；2.不是数字，是字符串。那么直接设置即可，并在后面进行检查。
    }

    // 特化处理：先把多边形的自动提参的参数加入参数管理器
    if(SHAPE_POLYGEN == m_shape_name)
    {
        auto extractB = [](const QString &C, const QString &A)
        {
            // 查找最后一个加号的位置
            int plusPos = C.lastIndexOf('+');
            if (plusPos == -1)
            {
                return QString();
            }

            // 提取加号后面的部分作为 B 字符串
            QString B = C.mid(plusPos + 1);

            // 检查 A 字符串是否匹配 C 字符串的前部分
            QString A_part = C.left(plusPos);
            if (A_part != A)
            {
                return QString();
            }

            return B;
        };

        QMap<QString, pm::ParamDecl> cache;
        for(auto& param : m_params)
        {
            const auto& param_key = param.key();
            const auto& param_exp = param.expression();
            cache[param_key] = param;
            if(pm::Expression::is_num(param_exp))
            {
                continue;
            }

            QRegularExpression regex("([xy])(\\d+)");
            QRegularExpressionMatch match = regex.match(param_key);

            if (!match.hasMatch())
            {
                continue;
            }

            QString character = match.captured(1); // 提取字符 'x' 或 'y'
            int number = match.captured(2).toInt();

            if(number == 1)
            {
                auto p = mp_pri->param_mgr()->find_param(param_exp);
                if(nullptr != p)
                {
                    p->set_value(param.value());
                    continue;
                }
                else
                {
                    emit add_new_param_to_param_mgr(param_exp, QString::number(param.value()), "");
                    mp_pri->param_mgr()->refresh_params_value();
                }
            }
            else
            {
                QString last_param_key = character + QString::number(number - 1);
                const auto& last_param = cache[last_param_key];

                if(!pm::Expression::is_num(last_param.expression()) && last_param.value() == param.value())  // 情况1：上一个参数不是数字，且和当前的参数的值一样，不需要增加参数，设置成一样的即可
                {
                    param.set_expression(last_param.expression());
                }
                else
                {
                    QString delat = extractB(param_exp, last_param.expression());
                    if(delat.isEmpty())
                    {
                        continue;
                    }

                    qreal dealt_value = param.value() - last_param.value();

                    auto p = mp_pri->param_mgr()->find_param(delat);
                    if(nullptr != p)
                    {
                        p->set_value(dealt_value);
                    }
                    else
                    {
                        emit add_new_param_to_param_mgr(delat, QString::number(dealt_value), "");
                        mp_pri->param_mgr()->refresh_params_value();
                    }
                }
            }
        }
    }

    // 检查输入
    QList<QLineEdit *> line_edits = findChildren<QLineEdit *>();
    for (QLineEdit *line_edit : line_edits)
    {
        auto line_edit_name = line_edit->objectName();
        if(line_edit_name.isEmpty())
        {
            continue;
        }
        auto text_str = line_edit->text();
        // 先检查是不是数字
        bool is_num = pm::Expression::is_num(text_str);
        bool is_exp = pm::Expression::isExpression(mp_pri->param_mgr(), text_str);

        if(false == is_num && false == is_exp)
        {
            m_params = params_cache;
            QString warning_info = "please check " + line_edit_name + " input!";
            QMessageBox::warning(this, "Warning", warning_info);
            return;
        }
    }

    for(auto layout : m_layouts)  // 检查曲线
    {
        if(false == layout.second)
        {
            continue;
        }

        if(false == layout.first->is_valid())
        {
            QString waring_info = "The input " + layout.first->param_name() + " is invalid, please input right string.";
            QMessageBox::warning(this, "Warning", waring_info);
            return;
        }
    }

    // 处理输入值
    if(m_shape_name == SHAPE_CURVE)
    {
        if(nullptr == mp_editing_tree_node)  // 创建
        {
            QVector<QPair<QString, QVariant>> param_list;
            for(auto layout : m_layouts)
            {
                if(false == layout.second)
                {
                    continue;
                }
                auto param_name = layout.first->param_name();
                auto param_str = layout.first->input_content();
                pm::ParamDecl param_exp(param_name, param_str);
                m_params.push_back(param_exp);

                auto reverse_name = layout.first->reverse_name();
                auto reverse_value = layout.first->reverse_value();
                pm::ParamDecl param_reverse(reverse_name, reverse_value);
                m_params.push_back(param_reverse);
            }
        }
        else // 编辑
        {
            for(auto layout : m_layouts)
            {
                if(false == layout.second)  // 被删除了
                {
                    for(auto itor = m_params.begin(); itor != m_params.end(); itor++) // 如果是已有的参数，则进行修改
                    {
                        if(itor->key() == layout.first->param_name())
                        {
                            m_params.erase(itor);
                            break;
                        }
                    }
                    continue;
                }
                auto param_name = layout.first->param_name();
                auto param_str = layout.first->input_content();
                pm::ParamDecl param_exp(param_name, param_str);

                auto reverse_name = layout.first->reverse_name();
                auto reverse_value = layout.first->reverse_value();
                pm::ParamDecl param_reverse(reverse_name, reverse_value);

                bool find_param = false;
                for(auto& param : m_params)  // 如果是已有的参数，则进行修改
                {
                    if(param.key() == param_name)
                    {
                        param = param_exp;
                        find_param = true;
                    }
                    else if(param.key() == reverse_name)
                    {
                        param = param_reverse;
                        find_param = true;
                    }
                }

                if(false == find_param)   // 如果是新的的参数，则进行添加
                {
                    m_params.push_back(param_exp);
                    m_params.push_back(param_reverse);
                }
            }
        }

    }

    QDialog::accept();
}

void pr::PriShapeDialog::add_equation_input_widget()
{
    m_add_equation_count++;
    PriCurveInputHBoxLayout * add_equation_layout = new PriCurveInputHBoxLayout(this, QString(SHAPE_CURVE_EQUATION), m_add_equation_count);
    auto succeed = connect(add_equation_layout, &PriCurveInputHBoxLayout::remove_current_layout, this, &pr::PriShapeDialog::remove_curve_input_layout);
    Q_ASSERT(succeed);
    //将所有行添加到主布局
    m_context_layout->addLayout(add_equation_layout);
    m_layouts.append(QPair<PriCurveInputHBoxLayout*, bool>(add_equation_layout, true));
}

void pr::PriShapeDialog::add_fixed_point_set_input_widget()
{
    m_add_fixed_point_count++;
    PriCurveInputHBoxLayout * add_fixed_point_set_layout = new PriCurveInputHBoxLayout(this, QString(SHAPE_CURVE_FIXED_POINT_SET), m_add_fixed_point_count);
    auto succeed = connect(add_fixed_point_set_layout, &PriCurveInputHBoxLayout::remove_current_layout, this, &pr::PriShapeDialog::remove_curve_input_layout);
    Q_ASSERT(succeed);
    //将所有行添加到主布局
    m_context_layout->addLayout(add_fixed_point_set_layout);
    m_layouts.append(QPair<PriCurveInputHBoxLayout*, bool>(add_fixed_point_set_layout, true));
}

void pr::PriShapeDialog::remove_curve_input_layout(PriCurveInputHBoxLayout *layout)
{
    auto succeed = disconnect(layout, &PriCurveInputHBoxLayout::remove_current_layout, this, &pr::PriShapeDialog::remove_curve_input_layout);
    Q_ASSERT(succeed);

    // 移除行
    m_context_layout->removeItem(layout);

    for(auto& recorded_layout : m_layouts)
    {
        if(recorded_layout.first == layout)
        {
            recorded_layout.second = false;
            break;
        }
    }
}
