#include "redoService.h"

#include <QKeyEvent>

#include "dbManager.h"
#include "dbObject.h"

namespace cm
{



RedoService::RedoService()
    : Service()
{
}

RedoService::~RedoService()
{
    // use mgr() to free
}

void RedoService::mgr(db::Manager *mgr, bool free_old)
{
    if(free_old && mp_mgr)
        delete mp_mgr;

    mp_mgr = mgr;
}

void RedoService::queue(db::Object *obj, db::Op *op)
{
    obj->manager(mp_mgr);
    mp_mgr->queue(obj, op);
}

void RedoService::undo()
{
    mp_mgr->undo();
}

void RedoService::redo()
{
    mp_mgr->redo();
}

void RedoService::transaction()
{
    mp_mgr->transaction("");
}

void RedoService::commit()
{
    mp_mgr->commit();
}

void RedoService::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers() != Qt::ControlModifier)
        return;

    if(event->key() == Qt::Key_Z)
    {
        undo();
    }else if(event->key() == Qt::Key_Y)
    {
        redo();
    }
}

RedoService *RedoService::instance()
{
    static RedoService s_instance;
    return &s_instance;
}



}
