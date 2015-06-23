#include "CommandsFactory.h"

CommandsFactory::CommandsFactory(QObject *parent)
    : QObject(parent)
{
}

CommandsFactory:: ~CommandsFactory() {
    Q_FOREACH(Command* cmd, commands) {
        // use deleteLater because all commands live in another thread
        cmd->deleteLater();
    }
}

void CommandsFactory::countCommand(Command* cmd) {
    commands.append(cmd);
}
