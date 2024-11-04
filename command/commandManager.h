#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include "cmdCommon.h"
#include <QUndoCommand>

namespace cmd
{
class CMD_PUBLIC CommandManager final
{
private:
    CommandManager();
    ~CommandManager();

public:
    static CommandManager* instance();

    void init();
    void push(QUndoCommand *cmd);  // 内存释放托管给m_cmd_stack

    void undo();
    void redo();

    int count() const;
    const QUndoStack& undo_stack() const;

private:
    QUndoStack m_cmd_stack;
};
}

#endif // COMMANDMANAGER_H
