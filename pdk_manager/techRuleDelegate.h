#ifndef TECHRULEDELEGATE_H
#define TECHRULEDELEGATE_H

#include <QStyledItemDelegate>
#include <QWidget>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QStyleOptionViewItem>
#include <QPainter>

namespace pdk
{
class TechRuleDelegate : public QStyledItemDelegate
{
public:
    explicit TechRuleDelegate(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    // void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};

}

#endif // TECHRULEDELEGATE_H
