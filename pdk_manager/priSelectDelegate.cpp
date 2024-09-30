#include <QToolButton>
#include <QComboBox>
#include <QLineEdit>
#include "priSelectDelegate.h"

namespace pdk
{

PriSelectDelegate::PriSelectDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

// 创建编辑器 (在右边显示 ComboBox)
QWidget* PriSelectDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() == 0)    // 右边是 ComboBox
    {
        QComboBox* comboBox = new QComboBox(parent);
        comboBox->addItem(QIcon(":/path/to/icon1.png"), "Option 1");
        comboBox->addItem(QIcon(":/path/to/icon2.png"), "Option 2");
        comboBox->addItem(QIcon(":/path/to/icon3.png"), "Option 3");
        return comboBox;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

// 设置编辑器数据
void PriSelectDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (QComboBox* comboBox = qobject_cast<QComboBox*>(editor))
    {
        int currentIndex = index.data(Qt::EditRole).toInt();
        comboBox->setCurrentIndex(currentIndex);
    }
}

// 将数据从编辑器设置回模型
void PriSelectDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (QComboBox* comboBox = qobject_cast<QComboBox*>(editor))
    {
        model->setData(index, comboBox->currentIndex(), Qt::EditRole);
    }
}

void PriSelectDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

#if 1
    // 获取模型中的数据
    QIcon lineEditIcon = index.data(Qt::DecorationRole).value<QIcon>();  // 图标
    QString lineEditText = index.data(Qt::DisplayRole).toString();  // 文本

    QIcon centerIcon = index.data(Qt::UserRole + 1).value<QIcon>();  // 中间图标
    QString comboBoxText = index.data(Qt::UserRole + 2).toString();  // ComboBox 文本
    QIcon comboBoxIcon = index.data(Qt::UserRole + 3).value<QIcon>();  // ComboBox 图标

    // 左边的 QLineEdit 模拟
    QRect lineEditRect = option.rect.adjusted(5, 5, -option.rect.width()/3 * 2, -5);
    painter->drawRect(lineEditRect);  // 模拟 QLineEdit 边框
    lineEditIcon.paint(painter, lineEditRect.adjusted(5, 5, -lineEditRect.width() + 25, -5));  // 左边的图标
    painter->drawText(lineEditRect.adjusted(30, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, lineEditText);  // 文本

    // 中间的图标
    QRect centerIconRect = option.rect.adjusted(option.rect.width()/3, 5, -option.rect.width()/3, -5);
    centerIcon.paint(painter, centerIconRect);

    // 右边的 ComboBox 模拟
    QRect comboBoxRect = option.rect.adjusted(option.rect.width()/3 * 2 + 5, 5, -5, -5);
    painter->drawRect(comboBoxRect);  // 模拟 ComboBox 边框
    comboBoxIcon.paint(painter, comboBoxRect.adjusted(5, 5, -comboBoxRect.width() + 25, -5));  // ComboBox 图标
    painter->drawText(comboBoxRect.adjusted(30, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, comboBoxText);  // ComboBox 文本
#endif

    // 左边的 QLineEdit 模拟
    // QIcon lineEditIcon = index.data(Qt::DecorationRole).value<QIcon>();
    // QString lineEditText = index.data(Qt::DisplayRole).toString();
    // QRect lineEditRect = option.rect.adjusted(5, 5, -option.rect.width()/3 * 2, -5);
    // painter->drawRect(lineEditRect);
    // lineEditIcon.paint(painter, lineEditRect.adjusted(5, 5, -lineEditRect.width() + 25, -5));  // 左边图标
    // painter->drawText(lineEditRect.adjusted(30, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, lineEditText);  // 文本

    // // 中间的图标
    // QIcon centerIcon = index.data(Qt::UserRole + 1).value<QIcon>();
    // QRect centerIconRect = option.rect.adjusted(option.rect.width()/3, 5, -option.rect.width()/3, -5);
    // centerIcon.paint(painter, centerIconRect);

    painter->restore();
}

QSize PriSelectDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(option.rect.width(), 40);  // 每行高度为 40
}


}
