#ifndef FAKECOMMAND_H
#define FAKECOMMAND_H


#include <remotedrive/RemoteCommand.h>
#include <QDebug>
#include <QThread>
namespace FakeCommands {
class FakeSyncCommand : public KeePassxDriveSync::RemoteCommand
{
    Q_OBJECT
public:
    FakeSyncCommand(){
    }
    ~FakeSyncCommand(){}

    //TODO add Q_DECL_OVERRIDE when migrate to QT5
    void execute(const QVariantMap options);
};

class FakeAsyncCommand : public KeePassxDriveSync::RemoteCommand
{
    Q_OBJECT
public:
    FakeAsyncCommand(){
    }
    ~FakeAsyncCommand(){}

    //TODO add Q_DECL_OVERRIDE when migrate to QT5
    void execute(const QVariantMap options);
};
class FakeFailCommand : public KeePassxDriveSync::RemoteCommand
{
    Q_OBJECT
public:
    FakeFailCommand(){}
    ~FakeFailCommand(){}

    //TODO add Q_DECL_OVERRIDE when migrate to QT5
    void execute(const QVariantMap options);
};
class FakeArgumentsCommand : public KeePassxDriveSync::RemoteCommand
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
