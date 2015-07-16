#ifndef COMMANDSFACTORY_H
#define COMMANDSFACTORY_H
#include <QtCore/QObject>
#include <QtCore/QVariantMap>
#include "remotedrive/Command.h"

class CommandsFactory : public QObject
{
    Q_OBJECT
public:
    CommandsFactory(QObject *parent);
    ~CommandsFactory();
    virtual KeePassxDriveSync::Command *download() = 0;
    virtual KeePassxDriveSync::Command *sync() = 0;
    virtual KeePassxDriveSync::Command *list() = 0;
    virtual KeePassxDriveSync::Command *upload() = 0;
    virtual KeePassxDriveSync::Command *remove() = 0;
private:
    // keep pointers to all initiated commands cause need to delete them manually
    // all commands live in another thread and do not support Qt memory management
    QList<KeePassxDriveSync::Command*> commands;
protected:
    KeePassxDriveSync::Command *countCommand(KeePassxDriveSync::Command* cmd);
};

#endif // COMMANDSFACTORY_H
