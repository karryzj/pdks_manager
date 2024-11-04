#ifndef STYLEFRAME_H
#define STYLEFRAME_H

#include <QFrame>
#include <QObject>
#include "gdsLayerInfo.h"

namespace pdk
{
class StyleFrame : public QFrame
{
    Q_OBJECT
public:
    explicit StyleFrame(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    StyleFrame(BorderStyle style, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~StyleFrame(void) override;

public:
    void setStyle(const BorderStyle &style);
    BorderStyle style() const;

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

signals:
    void dataChanged(const BorderStyle &style);

private:
    BorderStyle m_style;
};
}

#endif // STYLEFRAME_H
