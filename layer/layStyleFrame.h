#ifndef STYLEFRAME_H
#define STYLEFRAME_H

#include <QFrame>
#include <QObject>
#include "layInfo.h"

namespace ly
{

class LayStyleFrame final: public QFrame
{
    Q_OBJECT
public:
    explicit LayStyleFrame(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    LayStyleFrame(LayerInfo style, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~LayStyleFrame(void) override;

public:
    void setStyle(const LayerInfo &style);
    LayerInfo style() const;
    LayerInfo getOldStyle() const;

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

signals:
    void dataChanged(const LayerInfo &style);

private:
    LayerInfo m_style;
    LayerInfo m_old_style;
};
}

#endif // STYLEFRAME_H
