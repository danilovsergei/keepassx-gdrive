#ifndef COMMANDSFACTORY_H
#define COMMANDSFACTORY_H
#include <QtCore/QObject>

class CommandsFactory : public QObject
{
Q_OBJECT
public:
    CommandsFactory();
    virtual download(QVariantMap args) = 0;


};

#endif // COMMANDSFACTORY_H
