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
    virtual Command *download() = 0;
    virtual Command *sync() = 0;
    virtual Command *list() = 0;
};

#endif // COMMANDSFACTORY_H
