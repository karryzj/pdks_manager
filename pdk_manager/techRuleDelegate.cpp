#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include "techRuleDelegate.h"
#include "techRuleItemWidget.h"

namespace pdk
{
TechRuleDelegate::TechRuleDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

// 创建编辑器，左边显示 QLineEdit，右边显示两个按钮
QWidget *TechRuleDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    TechRuleItemWidget *editor = new TechRuleItemWidget("",parent);
    return editor;
}

// 将模型中的数据加载到编辑器中
void TechRuleDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    TechRuleItemWidget *item = qobject_cast<TechRuleItemWidget *>(editor);
    if (item)
    {
        // 获取模型中的数据
        QString text = index.model()->data(index, Qt::EditRole).toString();

        // 找到 CustomWidget 中的 QLineEdit，并设置初始文本内容
        QLineEdit *lineEdit = item->findChild<QLineEdit *>();
        if (lineEdit)
        {
            lineEdit->setText(text);  // 将模型中的数据设置到 QLineEdit
        }
    }

}

// 将编辑器中的数据设置回模型
void TechRuleDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    TechRuleItemWidget *item = qobject_cast<TechRuleItemWidget *>(editor);
    if (item)
    {
        // 获取 QLineEdit 的文本
        QLineEdit *lineEdit = item->findChild<QLineEdit *>();
        if (lineEdit)
        {
            QString text = lineEdit->text();

            // 将文本保存到模型的 index 位置
            model->setData(index, text, Qt::EditRole);
        }
    }
}

// 设置编辑器的尺寸
void TechRuleDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}


}
