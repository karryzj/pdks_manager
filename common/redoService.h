#ifndef REDOSERVICE_H
#define REDOSERVICE_H

#include "common.h"
#include "service.h"

namespace db
{
class Op;
class Object;
class Manager;
}

namespace cm
{

class CM_PUBLIC RedoService : public Service
{
    Q_OBJECT
public:
    RedoService();
    ~RedoService();

    void mgr(db::Manager *mgr, bool free_old = false);

    void queue(db::Object* obj, db::Op *op);
    void undo();
    void redo();

    void transaction();
    void commit();

    void keyPressEvent(QKeyEvent *event) override;

    static RedoService *instance();

private:
    db::Manager *mp_mgr;
};

}

#endif // REDOSERVICE_H
