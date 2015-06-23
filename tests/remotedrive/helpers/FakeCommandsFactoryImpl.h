#ifndef FAKECOMMANDSFACTORYIMPL_H
#define FAKECOMMANDSFACTORYIMPL_H

#include <remotedrive/CommandsFactory.h>
#include <remotedrive/helpers/FakeCommand.h>
#include <remotedrive/gdrive/CommandsFactoryImpl.h>
using namespace FakeCommands;

class FakeCommandsFactoryImpl: public CommandsFactoryImpl
{
    Q_OBJECT
public:
    FakeCommandsFactoryImpl(QObject *parent);

public Q_SLOTS:
    KeePassxDriveSync::Command* fakeSyncCommand();
    KeePassxDriveSync::Command* fakeAsyncCommand();
    KeePassxDriveSync::Command* fakeFailCommand();
    KeePassxDriveSync::Command* fakeArgumentsCommand();
};

#endif // FAKECOMMANDSFACTORYIMPL_H
