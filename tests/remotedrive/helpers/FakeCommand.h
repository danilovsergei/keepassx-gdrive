#ifndef FAKECOMMAND_H
#define FAKECOMMAND_H


#include <remotedrive/Command.h>
#include <QtCore/QDebug>
#include <QtCore/QThread>
namespace FakeCommands {
class FakeSyncCommand : public KeePassxDriveSync::Command
{
    Q_OBJECT
public:
    FakeSyncCommand(){
    }
    ~FakeSyncCommand(){}

    //TODO add Q_DECL_OVERRIDE when migrate to QT5
    void execute(const QVariantMap options);
};

class FakeAsyncCommand : public KeePassxDriveSync::Command
{
    Q_OBJECT
public:
    FakeAsyncCommand(){
    }
    ~FakeAsyncCommand(){}

    //TODO add Q_DECL_OVERRIDE when migrate to QT5
    void execute(const QVariantMap options);
};
class FakeFailCommand : public KeePassxDriveSync::Command
{
    Q_OBJECT
public:
    FakeFailCommand(){}
    ~FakeFailCommand(){}

    //TODO add Q_DECL_OVERRIDE when migrate to QT5
    void execute(const QVariantMap options);
};
class FakeArgumentsCommand : public KeePassxDriveSync::Command
{
    Q_OBJECT
public:
    FakeArgumentsCommand() {}
    ~FakeArgumentsCommand() {}

    //TODO add Q_DECL_OVERRIDE when migrate to QT5
    void execute(const QVariantMap options);
};

}
#endif // FAKECOMMAND_H
