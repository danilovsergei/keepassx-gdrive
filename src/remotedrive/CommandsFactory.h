#ifndef COMMANDSFACTORY_H
#define COMMANDSFACTORY_H
#include <QtCore/QObject>
#include <QtCore/QVariantMap>
#include "Command.h"
using namespace KeePassxDriveSync;
class CommandsFactory : public QObject
{
    Q_OBJECT
public:
    CommandsFactory(QObject *parent);
    ~CommandsFactory();
    virtual Command *download() = 0;
    virtual Command *sync() = 0;
    virtual Command *list() = 0;
    virtual Command *upload() = 0;
    virtual Command *remove() = 0;
private:
    // keep pointers to all initiated commands cause need to delete them manually
    // all commands live in another thread and do not support Qt memory management
    QList<Command*> commands;
protected:
    void countCommand(Command* cmd);
};

#endif // COMMANDSFACTORY_H
