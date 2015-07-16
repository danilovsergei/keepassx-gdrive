#ifndef TESTUPLOADDELETECOMMAND_H
#define TESTUPLOADDELETECOMMAND_H
#include <QtCore/QObject>
#include "tests.h"
#include <remotedrive/AuthCredentials.h>
#include <remotedrive/gdrive/GoogleDriveCredentials.h>
#include <remotedrive/CommandsFactory.h>
#include <GDriveTestUtils.h>
#include <QtCore/QScopedPointer>
#include <QtCore/QTemporaryFile>
#include "crypto/Crypto.h"
#include <QtCore/QDebug>
#include <remotedrive/Command.h>
#include <QtTest/QSignalSpy>
#include "config-keepassx-tests.h"
#include <core/Tools.h>

using namespace KeePassxDriveSync;

class TestUploadDeleteCommand : public QObject
{
    Q_OBJECT
public:
    TestUploadDeleteCommand();
private Q_SLOTS:
    void testDeleteOneFile();
    void  deleteTwoFiles();
    void deleleNonExistingFile();
    void uploadOneDb();
    void uploadTwoDb();
    void updateDbRevision();
    void testDownloadOneFile();
    void initTestCase();
private:
    RemoteDriveApi* remoteDriveApi = Q_NULLPTR;
    QScopedPointer<GDriveTestUtils> testUtils;
    QString parentDir = QString(GDRIVE_DIR);
    AuthCredentials *creds = Q_NULLPTR;
    CommandsFactory *commandsFactory = Q_NULLPTR;
};

#endif // TESTUPLOADDELETECOMMAND_H
