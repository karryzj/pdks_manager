#include "commandBase.h"

namespace cmd
{
CommandBase::CommandBase(QUndoCommand* parent) :QUndoCommand(parent)
{

}

CommandBase::~CommandBase()
{

}

void CommandBase::undo()
{

}

void CommandBase::redo()
{

}

}
