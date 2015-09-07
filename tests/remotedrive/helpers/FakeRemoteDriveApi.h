#ifndef FAKEREMOTEDRIVEAPI_H
#define FAKEREMOTEDRIVEAPI_H

#include <remotedrive/RemoteDriveApi.h>
#include <QObject>
#include <remotedrive/CommandsFactory.h>
#include <remotedrive/helpers/FakeCommandsFactoryImpl.h>

class FakeRemoteDriveApi : public RemoteDriveApi
{
Q_OBJECT
public:
    FakeRemoteDriveApi(QObject *parent, CommandsFactory *factoryImpl);
    KeePassxDriveSync::Command fakeSyncCommand();
    KeePassxDriveSync::Command fakeAsyncCommand();
    KeePassxDriveSync::Command fakeFailCommand();
    KeePassxDriveSync::Command fakeArgumentsCommand();

private:
    FakeCommandsFactoryImpl* fakeFactory = Q_NULLPTR;

};

#endif // FAKEREMOTEDRIVEAPI_H
