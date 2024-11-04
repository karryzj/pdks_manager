#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QColorDialog>
#include <QComboBox>
#include "colorlabel.h"
#include "styleDialog.h"

namespace pdk
{
StyleDialog::StyleDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent)
{
    setWindowTitle("Style");
    setupUi();
}

StyleDialog::StyleDialog(const BorderStyle &style, QWidget *parent, Qt::WindowFlags f):QDialog(parent), m_border_style(style)
{
    setWindowTitle("Style");
    setupUi(style);
}

StyleDialog::~StyleDialog()
{

}

void StyleDialog::setupUi()
{
    QLabel *label1 = new QLabel("外框样式：");
    QComboBox *board_style_combobox = new QComboBox();
    QStringList board_style;
    board_style << "SolidLine" << "DashLine" << "DotLine" << "DashDotLine" << "DashDotDotLine";
    board_style_combobox->addItems(board_style);
    QLabel *label2 = new QLabel("外框颜色：");
    ColorLabel *board_color_label = new ColorLabel();
    QLabel *label3 = new QLabel("填充样式：");
    QComboBox *fill_style_combobox = new QComboBox();
    QStringList fill_style;
    fill_style << "空" << "直线" << "竖线" << "网格线" << "左斜线" << "右斜线" << "斜线网络";
    fill_style_combobox->addItems(fill_style);
    QLabel *label4 = new QLabel("填充颜色：");
    ColorLabel *fill_color_label = new ColorLabel();

    QHBoxLayout *h1 = new QHBoxLayout();
    h1->addWidget(label1);
    h1->addWidget(board_style_combobox);
    QHBoxLayout *h2 = new QHBoxLayout();
    h2->addWidget(label2);
    h2->addWidget(board_color_label);
    QHBoxLayout *h3 = new QHBoxLayout();
    h3->addWidget(label3);
    h3->addWidget(fill_style_combobox);
    QHBoxLayout *h4 = new QHBoxLayout();
    h4->addWidget(label4);
    h4->addWidget(fill_color_label);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // 连接按钮框的信号到对话框的槽函数
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(board_style_combobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index)
    {
        if (m_border_style.border_line != index)
            m_border_style.border_line = index;
    });
    connect(fill_style_combobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index)
    {
        if (m_border_style.fill_line != index)
            m_border_style.fill_line = index;
    });
    connect(board_color_label, &ColorLabel::doubleClicked, [=]()
    {
        QColor color = QColorDialog::getColor(Qt::white);
        if (color.isValid())
        {
            // 获取颜色的RGB值
            int red = color.red();
            int green = color.green();
            int blue = color.blue();

            // 将RGB值组合成整数
            int rgbValue = (red << 16) | (green << 8) | blue;

            if (m_border_style.border_color != rgbValue)
                m_border_style.border_color = rgbValue;
            board_color_label->setStyleSheet(QString("background-color: %1;").arg(color.name()));
        }
    });
    connect(fill_color_label, &ColorLabel::doubleClicked, [=]()
    {
        QColor color = QColorDialog::getColor(Qt::white);
        if (color.isValid())
        {
            // 获取颜色的RGB值
            int red = color.red();
            int green = color.green();
            int blue = color.blue();

            // 将RGB值组合成整数
            int rgbValue = (red << 16) | (green << 8) | blue;
            if (m_border_style.fill_color != rgbValue)
                m_border_style.fill_color = rgbValue;
            fill_color_label->setStyleSheet(QString("background-color: %1;").arg(color.name()));
        }
    });

    QVBoxLayout *layout = new QVBoxLayout(this);
    // layout->addWidget(buttonBox);
    layout->addLayout(h1);
    layout->addLayout(h2);
    layout->addLayout(h3);
    layout->addLayout(h4);
    layout->addWidget(buttonBox);
}

void StyleDialog::setupUi(const BorderStyle &style)
{
    QLabel *label1 = new QLabel("外框样式：");
    QComboBox *board_style_combobox = new QComboBox();
    QStringList board_style;
    board_style << "SolidLine" << "DashLine" << "DotLine" << "DashDotLine" << "DashDotDotLine";
    board_style_combobox->addItems(board_style);
    board_style_combobox->setCurrentIndex(style.border_line);
    QLabel *label2 = new QLabel("外框颜色：");
    ColorLabel *board_color_label = new ColorLabel();
    // 获取颜色的RGB值
    int r = (style.border_color >> 16) & 0xff;
    int g = (style.border_color >> 8) & 0xff;
    int b = (style.border_color) & 0xff;
    QColor border_color(r, g, b);
    board_color_label->setStyleSheet(QString("background-color: %1;").arg(border_color.name(QColor::HexRgb)));
    QLabel *label3 = new QLabel("填充样式：");
    QComboBox *fill_style_combobox = new QComboBox();
    QStringList fill_style;
    fill_style << "空" << "直线" << "竖线" << "网格线" << "左斜线" << "右斜线" << "斜线网络";
    fill_style_combobox->addItems(fill_style);
    fill_style_combobox->setCurrentIndex(style.fill_line);
    QLabel *label4 = new QLabel("填充颜色：");
    ColorLabel *fill_color_label = new ColorLabel();
    // 获取颜色的RGB值
    int red = (style.fill_color >> 16) & 0xff;
    int green = (style.fill_color >> 8) & 0xff;
    int blue = (style.fill_color) & 0xff;
    QColor fill_color(red, green, blue);
    fill_color_label->setStyleSheet(QString("background-color: %1;").arg(fill_color.name(QColor::HexRgb)));
    QHBoxLayout *h1 = new QHBoxLayout();
    h1->addWidget(label1);
    h1->addWidget(board_style_combobox);
    QHBoxLayout *h2 = new QHBoxLayout();
    h2->addWidget(label2);
    h2->addWidget(board_color_label);
    QHBoxLayout *h3 = new QHBoxLayout();
    h3->addWidget(label3);
    h3->addWidget(fill_style_combobox);
    QHBoxLayout *h4 = new QHBoxLayout();
    h4->addWidget(label4);
    h4->addWidget(fill_color_label);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // 连接按钮框的信号到对话框的槽函数
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(board_style_combobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index)
    {
        if (m_border_style.border_line != index)
            m_border_style.border_line = index;
    });
    connect(fill_style_combobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index)
    {
        if (m_border_style.fill_line != index)
            m_border_style.fill_line = index;
    });
    connect(board_color_label, &ColorLabel::doubleClicked, [=]()
    {
        QColor color = QColorDialog::getColor(Qt::white);
        if (color.isValid())
        {
            // 获取颜色的RGB值
            int red = color.red();
            int green = color.green();
            int blue = color.blue();

            // 将RGB值组合成整数
            int rgbValue = (red << 16) | (green << 8) | blue;

            if (m_border_style.border_color != rgbValue)
                m_border_style.border_color = rgbValue;
            board_color_label->setStyleSheet(QString("background-color: %1;").arg(color.name()));
        }
    });
    connect(fill_color_label, &ColorLabel::doubleClicked, [=]()
    {
        QColor color = QColorDialog::getColor(Qt::white);
        if (color.isValid())
        {
            // 获取颜色的RGB值
            int red = color.red();
            int green = color.green();
            int blue = color.blue();

            // 将RGB值组合成整数
            int rgbValue = (red << 16) | (green << 8) | blue;
            if (m_border_style.fill_color != rgbValue)
                m_border_style.fill_color = rgbValue;
            fill_color_label->setStyleSheet(QString("background-color: %1;").arg(color.name()));
        }
    });

    QVBoxLayout *layout = new QVBoxLayout(this);
    // layout->addWidget(buttonBox);
    layout->addLayout(h1);
    layout->addLayout(h2);
    layout->addLayout(h3);
    layout->addLayout(h4);
    layout->addWidget(buttonBox);
}


BorderStyle StyleDialog::getData()
{
    return m_border_style;
}

void StyleDialog::setData(const BorderStyle &style)
{
    if (m_border_style != style)
    {
        m_border_style = style;
        // emit dataChanged();
    }
}

}
