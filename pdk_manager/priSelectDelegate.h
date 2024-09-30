#ifndef PRISELECTDELEGATE_H
#define PRISELECTDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QWidget>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QPainter>

namespace pdk
{

class PriSelectDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    PriSelectDelegate(QObject *parent = nullptr);
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

}
#endif // PRISELECTDELEGATE_H
