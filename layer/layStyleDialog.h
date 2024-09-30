#ifndef STYLEDIALOG_H
#define STYLEDIALOG_H

#include <QDialog>
#include <QLabel>
#include "layInfo.h"

namespace ly
{
class LayColorLabel final : public QLabel
{
    Q_OBJECT
public:
    LayColorLabel(QWidget *parent = nullptr);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    void doubleClicked();
};

class LayStyleDialog final : public QDialog
{
    Q_OBJECT
public:
    explicit LayStyleDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    LayStyleDialog(const LayerInfo &style, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~LayStyleDialog(void) override;

    LayerInfo getData(void);
    void setData(const LayerInfo &style);
private:
    void setupUi(void);
    void setupUi(const LayerInfo &style);

// signals:
//     void dataChanged();

private:
    LayerInfo m_border_style;
};
}
#endif // STYLEDIALOG_H
