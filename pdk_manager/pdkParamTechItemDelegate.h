#ifndef PDKPARAMTECHITEMDELEGATE_H
#define PDKPARAMTECHITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>

namespace pdk
{
class PdkParamTechItemDelegate : public QStyledItemDelegate
{
public:
    explicit PdkParamTechItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

}
#endif // PDKPARAMTECHITEMDELEGATE_H
