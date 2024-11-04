#ifndef COLORLABEL_H
#define COLORLABEL_H

#include <QLabel>
#include <QObject>
#include <QMouseEvent>
namespace pdk
{

class ColorLabel : public QLabel
{
    Q_OBJECT

public:
    ColorLabel(QWidget *parent = nullptr);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    void doubleClicked();
};
}
#endif // COLORLABEL_H
