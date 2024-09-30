#ifndef SERVICE_H
#define SERVICE_H

#include <QKeyEvent>

namespace cm
{

class Service : public QObject
{
    Q_OBJECT
public:
    Service();
    ~Service();

    virtual void keyPressEvent(QKeyEvent *event) = 0;
    // can add more event.

};

}

#endif // SERVICE_H
