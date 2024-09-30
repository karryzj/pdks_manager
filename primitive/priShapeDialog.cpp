#include "priShapeDialog.h"
#include "Primitive.h"
#include "attachTreeNodeMgr.h"
#include "paramMgr.h"
#include "primitiveDefines.h"
#include <PriLayerInfoComboBox.h>

#include <ParamDecl.h>
#include <ParamUtils.h>
#include <ParamValidator.h>
#include <PriUtils.h>
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

using namespace pr;

PriShapeDialog::PriShapeDialog(const QString& shape_name,
                               bool add_type,
                               bool add_direction,
                               bool add_layer,
                               ly::LayerWidget* layer_widget,
                               Primitive* pri)
    : QDialog()
    , m_shape_name(shape_name)
    , m_add_type(add_type)
    , m_add_direction(add_direction)
    , m_add_layer(add_layer)
    , mp_layer_widget(layer_widget)
    , mp_pri(pri)
{
    init();
}

pr::PriShapeDialog::~PriShapeDialog()
{
}

void PriShapeDialog::update_all()
{
    auto combo_box_type = findChild<QComboBox*>(PRI_TYPE_TYPEOPTION);
    Q_ASSERT(combo_box_type);
    auto combo_box_layer_info = findChild<PriLayerInfoComboBox*>(PRI_LAYER_TYPEOPTION);
    Q_ASSERT(combo_box_layer_info);
    combo_box_layer_info->clear();
    if (combo_box_type->currentText() == tr(PRI_TYPE_LOCATION))
    {
        combo_box_layer_info->display_only_location_layer_info_item();
    }
    else if (combo_box_type->currentText() == tr(PRI_TYPE_ADD) || combo_box_type->currentText() == tr(PRI_TYPE_DELETE))
    {
        combo_box_layer_info->display_only_vaild_layer_info_item();
    }
}

void pr::PriShapeDialog::init()
{
    // 创建UI面板
    this->setWindowTitle(QString(QObject::tr(PRI_WINDOW_TITLE_PREFIX)) + QString(" ") + m_shape_name);  // 设置面板名
    this->setFixedSize(PRI_DIALOG_WIDTH_SIZE, PRI_DIALOG_HEIGHT_SIZE);

    QWidget *content_widget = new QWidget(this);
    QVBoxLayout* context_layout = new QVBoxLayout(
        content_widget);  // 用于将子控件垂直排列在一个父控件中。它会自动管理子控件的位置和大小，使它们在父控件中按从上到下的顺序排列。

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
        if(m_shape_name == SHAPE_POLYGEN)  // 额外增加一个勾选框
        {
            bool param_default_value = false;
            //增加参数的label
            QLabel * label = new QLabel(tr(SHAPE_POLYGEN_IS_EXTRACT_PARAM));

            //增加参数的
            auto check_box = new QCheckBox(this);
            check_box->setObjectName(tr(SHAPE_POLYGEN_IS_EXTRACT_PARAM));
            check_box->setChecked(param_default_value);

            connect(check_box, &QCheckBox::toggled, this, &PriShapeDialog::update_polygen_point_param_display);

            //增加一个LayerLayout，将两个label和输入框放在一行
            QHBoxLayout * layer_layout = new QHBoxLayout(this);

            //将label和输入框添加到布局中
            layer_layout->addWidget(label);
            layer_layout->addWidget(check_box);
            //将所有行添加到主布局
            context_layout->addLayout(layer_layout);
        }
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
                //将所有行添加到主布局
                context_layout->addLayout(param_layout);
            }
            else
            {
                Q_ASSERT(false);   // HINT@leixunyong。走到这里来的时候说明需要处理一些新增类型。
            }
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
    connect(ok_button, & QPushButton::clicked, this, & PriShapeDialog::check_input);

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

QVector<pm::ParamDecl> pr::PriShapeDialog::shape_param_list() const
{
    if(m_params.isEmpty())
    {
        // 获取当前形状的参数列表
        sp::ShapeDecl* current_shape_info = sp::ShapeFactory::instance()->get_shapes_decl()[m_shape_name];
        const auto& param_list = current_shape_info->param_list();
        QVector<pm::ParamDecl> params;
        // 遍历参数列表
        for (auto it = param_list.begin(); it != param_list.end(); ++it)
        {
            const QString& param_name = (* it).first;
            QLineEdit* line_edit = findChild<QLineEdit*>(param_name);
            Q_ASSERT(line_edit);
            pm::ParamDecl pd = pm::ParamDecl(param_name, line_edit->text());
            params.push_back(pd);
        }
        m_params = params;
    }

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
    auto layer_info = mp_editing_tree_node->layer_info();
    auto param_list = mp_editing_tree_node->params();

    set_shape_node_type(node_type);
    set_shape_node_direction(node_direction);
    set_shape_layer_info_name(layer_info);

    QVector<QPair<QString, QVariant>> param_pairs;
    for(auto param : param_list)
    {
        QPair<QString, QVariant> new_pair {param.key(), param.expression()};
        param_pairs.push_back(new_pair);
    }
    set_shape_param_list(param_pairs);

    // HINT@leixunyong。针对多边形的特化
    if(m_shape_name == SHAPE_POLYGEN)
    {
        auto check_box = findChild<QCheckBox*>(tr(SHAPE_POLYGEN_IS_EXTRACT_PARAM));
        check_box->setEnabled(!check_shape_is_extracted_params(param_list));
    }
}

bool pr::PriShapeDialog::is_extract_params() const
{
    return m_is_extract_params;
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
    bool is_extracted_param = false;
    for (auto idx = 0; idx < param_list.size(); idx++)
    {
        const QString& param_name = param_list[idx].first;
        const QVariant& param_default_value = param_list[idx].second;
        if(param_default_value.type() == QVariant::Double || param_default_value.type() == QVariant::String)
        {
            QLineEdit* lineEdit = findChild<QLineEdit*>(param_name);

            QString text_str = param_default_value.toString();
            if(!lineEdit)
            {
                continue;
            }
            text_str = pm::ParamUtils::str_to_3_pre_str(text_str);
            lineEdit->setText(text_str);
            if(SHAPE_POLYGEN == m_shape_name)
            {
                if(lineEdit->objectName() == SHAPE_ROTATE)
                {
                    continue;
                }

                if(pm::Expression::is_expression(text_str))
                {
                    is_extracted_param = true;
                }
            }
        }
        else
        {
            Q_ASSERT(false);  // HINT@leixunyong。走到这里来说明类型有问题。
        }
    }

    if(is_extracted_param)
    {
        for (auto idx = 0; idx < param_list.size(); idx++)
        {
            const QString& param_name = param_list[idx].first;
            QLineEdit* lineEdit = findChild<QLineEdit*>(param_name);
            if(lineEdit)
            {
                lineEdit->setEnabled(false);
            }
        }
    }
}

bool pr::PriShapeDialog::check_shape_is_extracted_params(const QVector<pm::ParamDecl> &param_list)
{
    if(m_shape_name == SHAPE_POLYGEN)
    {
        for(auto param : param_list)
        {
            const auto& param_name = param.key();
            if(param_name == SHAPE_ROTATE)
            {
                continue;
            }

            if(param.is_expression(mp_pri->param_mgr()))
            {
                return true; // 只要有一个是表达式，说明都是表达式
            }
        }
    }
    return false;
}

void PriShapeDialog::setup_connect()
{
    auto combo_box_type = findChild<QComboBox*>(PRI_TYPE_TYPEOPTION);
    Q_ASSERT(combo_box_type);
    auto combo_box_layer_info = findChild<PriLayerInfoComboBox*>(PRI_LAYER_TYPEOPTION);
    Q_ASSERT(combo_box_layer_info);
    connect(combo_box_type, QOverload<int>::of( & QComboBox::currentIndexChanged),
            this, & PriShapeDialog::update_combo_box_layer_info);
}

void PriShapeDialog::update_combo_box_layer_info()
{
    update_all();
}

void pr::PriShapeDialog::update_polygen_point_param_display(bool checked)
{
    // 能够走到这里来，一定是通过创建
    int node_id = (mp_editing_tree_node == nullptr ?  mp_pri->tree_node_mgr()->id_number() + 1 : mp_editing_tree_node->id());
    m_is_extract_params = checked;
    // 这里是拷贝构造，因此不会真实修改参数列表
    auto param_list = shape_param_list();
    param_list.removeAt(0);

    // 不打钩的时候，仅显示值即可
    if(!checked)
    {
        for(auto param : param_list)
        {
            QString param_name = param.key();
            auto param_line_edit = findChild<QLineEdit*>(param_name);
            if(param_line_edit)
            {
                param_line_edit->setText(QString::number( m_line_edit_name_to_value[param_name]));
                param_line_edit->setEnabled(true);
            }
        }
        return;
    }

    // 备份值
    if(checked)
    {
        m_line_edit_name_to_value.clear();
        for(const auto& param : param_list)
        {
            QString line_edit_name = param.key();
            qreal value = param.to_double(mp_pri->param_mgr());
            m_line_edit_name_to_value.insert(line_edit_name, value);
        }
    }


    // 设置line edit的值
    if(checked)
    {
        auto params = PriUtils::organize_and_eliminate_params(param_list, node_id);
        for(auto param : params)
        {
            const auto& param_name = param.key();
            auto line_edit = findChild<QLineEdit*>(param_name);
            Q_ASSERT(line_edit);
            line_edit->setText(param.expression());
            line_edit->setEnabled(false);
        }
    }
}

bool isNumeric(const QString &str)
{
    QRegularExpression re("^\\d+$");
    QRegularExpressionMatch match = re.match(str);
    return match.hasMatch();
}

void pr::PriShapeDialog::check_input()
{
    if(m_shape_name != SHAPE_POLYGEN)
    {
        QList<QLineEdit *> line_edits = findChildren<QLineEdit *>();

        // 输出所有 QLineEdit 的内容
        for (QLineEdit *line_edit : line_edits)
        {
            auto  text_str = line_edit->text();
            // 先检查是不是数字
            bool is_num = pm::Expression::is_num(text_str);
            bool is_param = mp_pri->param_mgr()->find_param(text_str);
            qreal value =  pm::Expression(text_str).to_double(mp_pri->param_mgr());

            if(false == is_num && false == is_param && 0 == value)
            {
                QString warning_info = "please check " + line_edit->objectName() + " input!";
                QMessageBox::warning(this, "Warning", warning_info);
                return;
            }
        }
    }
    QDialog::accept();
}
