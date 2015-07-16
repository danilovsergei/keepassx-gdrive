#ifndef TestCommand_H
#define TestCommand_H

#include <QtCore/QObject>
#include "tests.h"

#include <remotedrive/AuthCredentials.h>
#include <remotedrive/CommandsFactory.h>
#include <remotedrive/gdrive/CommandsFactoryImpl.h>
#include <remotedrive/RemoteDriveApi.h>
#include <remotedrive/gdrive/GoogleDriveCredentials.h>
#include <remotedrive/helpers/FakeCommandsFactoryImpl.h>
#include <remotedrive/helpers/FakeRemoteDriveApi.h>
#include <QtTest/QSignalSpy>

class TestCommand : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void testSyncCommandExecution();
    void testAsyncCommandExecution();
    void testFailCommandExecution();
    void testArgumentsCommandExecution();
private:
     CommandsFactory *commandsFactory =Q_NULLPTR;
     FakeRemoteDriveApi *remoteDriveApi = Q_NULLPTR;
};

#endif // TestCommand_H
