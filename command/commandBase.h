#ifndef COMMANDBASE_H
#define COMMANDBASE_H

#include "cmdCommon.h"
#include <QUndoCommand>
#include <QUndoStack>

namespace cmd
{

class CMD_PUBLIC CommandBase : public QUndoCommand
{
public:
    CommandBase(QUndoCommand* parent = nullptr);
    virtual ~CommandBase();
    void undo() override;
    void redo() override;
};

}
#endif // COMMANDBASE_H
