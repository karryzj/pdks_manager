#ifndef GDSMODEWIDGET_H
#define GDSMODEWIDGET_H

#include <QWidget>

namespace pdk
{

class GdsModeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GdsModeWidget(QWidget *parent = nullptr);
    ~GdsModeWidget();

private:
    void setupUi();
signals:
};

}

#endif // GDSMODEWIDGET_H
