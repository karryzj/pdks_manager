#include "colorLabel.h"

namespace pdk
{

ColorLabel::ColorLabel(QWidget *parent) : QLabel(parent)
{
    setStyleSheet("border: 2px solid black; padding: 5px;");
}

void ColorLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit doubleClicked();
    }
}

}
