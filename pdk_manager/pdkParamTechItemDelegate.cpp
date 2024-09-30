#include "pdkParamTechItemDelegate.h"

namespace pdk
{
PdkParamTechItemDelegate::PdkParamTechItemDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{}

void PdkParamTechItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    // 绘制背景
    painter->fillRect(option.rect, option.palette.base());

    // 获取模型中的数据
    QString lineEditText = index.data(Qt::DisplayRole).toString();
    QString labelTopText = index.data(Qt::UserRole + 1).toString();
    QString labelBottomText = index.data(Qt::UserRole + 2).toString();

    // 左边的 QLineEdit
    QRect lineEditRect = option.rect.adjusted(5, 5, -option.rect.width()/2, -5);  // 调整 QLineEdit 的位置
    painter->drawRect(lineEditRect);  // 模拟 QLineEdit 边框
    painter->drawText(lineEditRect, Qt::AlignVCenter | Qt::AlignLeft, lineEditText);

    // 右边两个 QLabel（上一个和下一个）
    QRect labelTopRect = option.rect.adjusted(option.rect.width()/2 + 5, 5, -5, -(option.rect.height()/2 + 2));
    QRect labelBottomRect = option.rect.adjusted(option.rect.width()/2 + 5, option.rect.height()/2, -5, -5);

    painter->drawText(labelTopRect, Qt::AlignLeft | Qt::AlignTop, labelTopText);
    painter->drawText(labelBottomRect, Qt::AlignLeft | Qt::AlignBottom, labelBottomText);

    painter->restore();
}

QSize PdkParamTechItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(option.rect.width(), 50);  // 每个项目高度为 50
}

}
