#include "priRulerDialog.h"
#include "attachTreeUtils.h"
#include "primitiveDefines.h"
#include "primitive.h"

#include <AttachTreeNode.h>
#include <PriGraphicsView.h>
#include <QClipboard>
#include <QCloseEvent>
#include <ShapePointGraphicsItem.h>
#include <cmath>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>

#include "math.h"

using namespace  pr;

PriRulerDialog::PriRulerDialog(Primitive* pri, PriGraphicsView* view, QWidget *parent)
    : QDialog(parent)
    , mp_pri(pri)
    , mp_view(view)
{
    setWindowTitle(PRI_RULER_DISTANCE_NAME);
    setObjectName(PRI_RULER_DISTANCE_NAME);
    init();
    set_connect();
}

PriRulerDialog::~PriRulerDialog()
{
    remove_connect();
    if(m_distance_arrow)
    {
        mp_view->scene()->removeItem(m_distance_arrow);
        delete m_distance_arrow;
    }
}

void PriRulerDialog::init()
{
    QVBoxLayout* main_layout = new QVBoxLayout(this);
    QGridLayout *grid_layout = new QGridLayout(this);
    main_layout->addLayout(grid_layout);

    QLabel* direction_label = new QLabel(tr(PRI_DIRECTION_LABEL_NAME), this);
    QLabel* expression_label = new QLabel(tr(PRI_EXPRESSION_LABEL_NAME), this);
    QLabel* value_label = new QLabel(tr(PRI_VALUE_LABEL_NAME), this);
    QLabel* copy_expression_label = new QLabel(tr(PRI_COPY_EXPRESSION_LABEL_NAME), this);
    grid_layout->addWidget(direction_label, 0, 0, Qt::AlignCenter);
    grid_layout->addWidget(expression_label, 0, 1, Qt::AlignCenter);
    grid_layout->addWidget(value_label, 0, 2, Qt::AlignCenter);
    grid_layout->addWidget(copy_expression_label, 0, 3, Qt::AlignCenter);

    QLabel* x_distance_label = new QLabel(tr(PRI_X_DISTANCE_LABEL_NAME), this);
    QLineEdit* x_expression_line_edit = new QLineEdit( this);
    x_expression_line_edit->setObjectName(tr(PRI_X_DISTANCE_EXPRSSION_LINE_EDIT_NAME));
    x_expression_line_edit->setEnabled(false);
    x_expression_line_edit->setFixedWidth(300);
    QLineEdit* x_value_line_edit = new QLineEdit( this);
    x_value_line_edit->setObjectName(tr(PRI_X_DISTANCE_VALUE_LINE_EDIT_NAME));
    x_value_line_edit->setEnabled(false);
    QPushButton *copy_x_expression_button = new QPushButton(tr(PRI_COPY_X_EXPRESSION_LABEL_NAME), this);
    connect(copy_x_expression_button, &QPushButton::clicked, this, [x_expression_line_edit]()
    {
        // 获取 QLineEdit 的内容
        QString text = x_expression_line_edit->text();
        // 将内容复制到剪贴板
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(text);
        // 弹窗提示已经复制了文本内容
        QMessageBox::information(nullptr, "Copied", "The expression of Δx has been copied to clipboard.");
    });
    grid_layout->addWidget(x_distance_label, 1, 0, Qt::AlignCenter);
    grid_layout->addWidget(x_expression_line_edit, 1, 1, Qt::AlignCenter);
    grid_layout->addWidget(x_value_line_edit, 1, 2, Qt::AlignCenter);
    grid_layout->addWidget(copy_x_expression_button, 1, 3, Qt::AlignCenter);

    QLabel* y_distance_label = new QLabel(tr(PRI_Y_DISTANCE_LABEL_NAME), this);
    QLineEdit* y_expression_line_edit = new QLineEdit( this);
    y_expression_line_edit->setObjectName(tr(PRI_Y_DISTANCE_EXPRSSION_LINE_EDIT_NAME));
    y_expression_line_edit->setEnabled(false);
    y_expression_line_edit->setFixedWidth(300);
    QLineEdit* y_value_line_edit = new QLineEdit( this);
    y_value_line_edit->setObjectName(tr(PRI_Y_DISTANCE_VALUE_LINE_EDIT_NAME));
    y_value_line_edit->setEnabled(false);
    QPushButton *copy_y_expression_button = new QPushButton(tr(PRI_COPY_Y_EXPRESSION_LABEL_NAME), this);
    connect(copy_y_expression_button, &QPushButton::clicked, this, [y_expression_line_edit]()
    {
        // 获取 QLineEdit 的内容
        QString text = y_expression_line_edit->text();
        // 将内容复制到剪贴板
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(text);
        // 弹窗提示已经复制了文本内容
        QMessageBox::information(nullptr, "Copied", "The expression of Δy has been copied to clipboard.");
    });
    grid_layout->addWidget(y_distance_label, 2, 0, Qt::AlignCenter);
    grid_layout->addWidget(y_expression_line_edit, 2, 1, Qt::AlignCenter);
    grid_layout->addWidget(y_value_line_edit, 2, 2, Qt::AlignCenter);
    grid_layout->addWidget(copy_y_expression_button, 2, 3, Qt::AlignCenter);

    QLabel* distance_label = new QLabel(tr(PRI_DISTANCE_LINE_EDIT_NAME), this);
    QLineEdit* expression_line_edit = new QLineEdit(this);
    expression_line_edit->setObjectName(tr(PRI_DISTANCE_EXPRESSION_LINE_EDIT_NAME));
    expression_line_edit->setEnabled(false);
    expression_line_edit->setFixedWidth(300);
    QLineEdit* value_line_edit = new QLineEdit( this);
    value_line_edit->setObjectName(tr(PRI_DISTANCE_VALUE_LINE_EDIT_NAME));
    value_line_edit->setEnabled(false);
    QPushButton *copy_distance_expression_button = new QPushButton(tr(PRI_COPY_DISTANCE_EXPRESSION_LABEL_NAME), this);
    connect(copy_distance_expression_button, &QPushButton::clicked, this, [expression_line_edit]()
    {
        // 获取 QLineEdit 的内容
        QString text = expression_line_edit->text();
        // 将内容复制到剪贴板
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(text);
        // 弹窗提示已经复制了文本内容
        QMessageBox::information(nullptr, "Copied", "The expression of distance has been copied to clipboard.");
    });

    grid_layout->addWidget(distance_label, 3, 0, Qt::AlignCenter);
    grid_layout->addWidget(expression_line_edit, 3, 1, Qt::AlignCenter);
    grid_layout->addWidget(value_line_edit, 3, 2, Qt::AlignCenter);
    grid_layout->addWidget(copy_distance_expression_button, 3, 3, Qt::AlignCenter);
}

void pr::PriRulerDialog::set_connect()
{
    if(false == m_has_connects)
    {
        bool succeed = false;
        succeed = connect(mp_view, &PriGraphicsView::mouse_left_button_press_item, this, &PriRulerDialog::setup_point_item);
        Q_ASSERT(succeed);

        m_has_connects = true;
    }
}

void pr::PriRulerDialog::remove_connect()
{
    if(true == m_has_connects)
    {
        bool succeed = false;
        succeed = disconnect(mp_view, &PriGraphicsView::mouse_left_button_press_item, this, &PriRulerDialog::setup_point_item);
        Q_ASSERT(succeed);
        m_has_connects = false;
    }
}

void pr::PriRulerDialog::closeEvent(QCloseEvent *event)
{
    done(0);

    if(m_distance_arrow)
    {
        m_distance_arrow->setVisible(false);
    }

    remove_connect();

    event->accept();
}

void pr::PriRulerDialog::setup_point_item(QGraphicsItem *item)
{
    sp::ShapePointGraphicsItem* point_item = dynamic_cast<sp::ShapePointGraphicsItem*>(item);
    if(nullptr == point_item)
    {
        return;
    }

    if(point_item->shape_item() == nullptr)
    {
        QMessageBox::warning(this, "Warning", "You selected the origin point, please select the right point again");
        mp_second_point_item = nullptr;
        return;
    }

    if(nullptr == mp_first_point_item)
    {
        mp_first_point_item = point_item;
        return;
    }
    else if(nullptr == mp_second_point_item)
    {
        mp_second_point_item = point_item;
    }
    else
    {
        return;
    }

    if(mp_first_point_item == mp_second_point_item)
    {
        QMessageBox::warning(this, "Warning", "You selected the point twice, please select the second point again");
        mp_second_point_item = nullptr;
        return;
    }

    auto start_point = mp_first_point_item->pos();
    auto end_point = mp_second_point_item->pos();

    m_distance_arrow = new pr::ArrowLineItem(start_point, end_point,  "distance");

    mp_view->scene()->addItem(m_distance_arrow);

    at::AttachTreeNode* first_tree_node = dynamic_cast<at::AttachTreeNode*>(at::AttachTreeUtils::attach_tree_node_point_item_in(mp_first_point_item, mp_pri->at_root()));
    Q_ASSERT(first_tree_node);
    int first_point_idx = at::AttachTreeUtils::point_item_index_in_tree_node(first_tree_node, mp_first_point_item);
    at::AttachTreeNode* second_tree_node = dynamic_cast<at::AttachTreeNode*>(at::AttachTreeUtils::attach_tree_node_point_item_in(mp_second_point_item, mp_pri->at_root()));
    Q_ASSERT(second_tree_node);
    int second_point_idx = at::AttachTreeUtils::point_item_index_in_tree_node(second_tree_node, mp_second_point_item);

    pm::Expression delat_x_expression = at::AttachTreeUtils::x_distance_exp( second_tree_node, second_point_idx, first_tree_node, first_point_idx);
    qreal delat_x_value = delat_x_expression.to_double( mp_pri->param_mgr());

    pm::Expression delat_y_expression = at::AttachTreeUtils::y_distance_exp(second_tree_node, second_point_idx, first_tree_node, first_point_idx);
    qreal delat_y_value = delat_y_expression.to_double( mp_pri->param_mgr());

    pm::Expression distance_expression = at::AttachTreeUtils::distance_exp(second_tree_node, second_point_idx, first_tree_node, first_point_idx);
    qreal distance_value = distance_expression.to_double(mp_pri->param_mgr());

    QLineEdit* x_expression_line_edit = findChild<QLineEdit*>(PRI_X_DISTANCE_EXPRSSION_LINE_EDIT_NAME);
    Q_ASSERT(x_expression_line_edit);
    x_expression_line_edit->setText(delat_x_expression.to_str());

    QLineEdit* x_value_line_edit = findChild<QLineEdit*>(PRI_X_DISTANCE_VALUE_LINE_EDIT_NAME);
    Q_ASSERT(x_value_line_edit);
    x_value_line_edit->setText(QString:: number(delat_x_value));

    QLineEdit* y_expression_line_edit = findChild<QLineEdit*>(PRI_Y_DISTANCE_EXPRSSION_LINE_EDIT_NAME);
    Q_ASSERT(y_expression_line_edit);
    y_expression_line_edit->setText(delat_y_expression.to_str());

    QLineEdit* y_value_line_edit = findChild<QLineEdit*>(PRI_Y_DISTANCE_VALUE_LINE_EDIT_NAME);
    Q_ASSERT(y_value_line_edit);
    y_value_line_edit->setText(QString:: number(delat_y_value));

    QLineEdit* distance_expression_line_edit = findChild<QLineEdit*>(PRI_DISTANCE_EXPRESSION_LINE_EDIT_NAME);
    Q_ASSERT(distance_expression_line_edit);
    distance_expression_line_edit->setText(distance_expression.to_str());

    QLineEdit* distance_value_line_edit = findChild<QLineEdit*>(PRI_DISTANCE_VALUE_LINE_EDIT_NAME);
    Q_ASSERT(distance_value_line_edit);
    distance_value_line_edit->setText(QString:: number(distance_value));

    exec();
}



/***************************************************************************************************************************/
pr::ArrowLineItem::ArrowLineItem(const QPointF &start, const QPointF &end, const QString &text, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_start_point(start)
    , m_end_point(end)
    , m_text(text)
{

}

pr::ArrowLineItem::~ArrowLineItem()
{

}

QRectF pr::ArrowLineItem::boundingRect() const
{
    // 返回项的边界矩形
    return QRectF(m_start_point, m_end_point).normalized();
}

void pr::ArrowLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen(Qt::black, 2, Qt::DashLine);  // 设置画笔颜色为黑色，宽度为2，样式为虚线
    painter->setPen(pen);

    // 绘制线条
    painter->drawLine(m_start_point, m_end_point);

    // 绘制箭头
    draw_arrow(painter, m_start_point, m_end_point);

    // 绘制文本
    draw_text_on_line(painter, m_start_point, m_end_point, m_text);
}

void pr::ArrowLineItem::draw_arrow(QPainter *painter, const QPointF &start, const QPointF &end)
{
    // 计算箭头的角度
    double angle = std::atan2(end.y() - start.y(), end.x() - start.x());

    // 箭头的长度和角度
    int arrowLength = 30;
    double arrowAngle = 3.14159265358979323846 / 6; // 30度

    // 箭头的两个点
    QPointF arrowP1 = end - QPointF(arrowLength * std::cos(angle - arrowAngle),
                                    arrowLength * std::sin(angle - arrowAngle));
    QPointF arrowP2 = end - QPointF(arrowLength * std::cos(angle + arrowAngle),
                                    arrowLength * std::sin(angle + arrowAngle));

    // 绘制箭头
    painter->drawLine(end, arrowP1);
    painter->drawLine(end, arrowP2);
}

void pr::ArrowLineItem::draw_text_on_line(QPainter *painter, const QPointF &start, const QPointF &end, const QString &text)
{
    // 计算文本的位置
    QPointF textPos = (start + end) / 2;

    // 设置字体
    QFont font = painter->font();
    font.setPointSize(12);
    painter->setFont(font);

    // 绘制文本
    painter->drawText(textPos, text);
}
