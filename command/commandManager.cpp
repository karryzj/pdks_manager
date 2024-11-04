#include "commandManager.h"

using namespace cmd;

CommandManager::CommandManager()
{

}

CommandManager::~CommandManager()
{

}

CommandManager *CommandManager::instance()
{
    static CommandManager _instance;
    return &_instance;
}

void CommandManager::init()
{
    m_cmd_stack.clear();
}

void CommandManager::push(QUndoCommand *cmd)
{
    Q_ASSERT(cmd);
    m_cmd_stack.push(cmd);
}

void CommandManager::undo()
{
    m_cmd_stack.undo();
}

void CommandManager::redo()
{
    m_cmd_stack.redo();
}

int CommandManager::count() const
{
    return m_cmd_stack.count();
}

const QUndoStack &CommandManager::undo_stack() const
{
    return m_cmd_stack;
}
