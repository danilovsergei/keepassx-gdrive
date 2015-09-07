#ifndef TESTUPLOADDELETECOMMAND_H
#define TESTUPLOADDELETECOMMAND_H
#include <QObject>
#include "tests.h"
#include <remotedrive/AuthCredentials.h>
#include <remotedrive/gdrive/GoogleDriveCredentials.h>
#include <remotedrive/CommandsFactory.h>
#include <GDriveTestUtils.h>
#include <QScopedPointer>
#include <QTemporaryFile>
#include "crypto/Crypto.h"
#include <QDebug>
#include <remotedrive/RemoteCommand.h>
#include <QSignalSpy>
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
    void testDownloadTimeStamp();
    void initTestCase();
private:
    RemoteDriveApi* remoteDriveApi = Q_NULLPTR;
    QScopedPointer<GDriveTestUtils> testUtils;
    QString parentDir = QString(GDRIVE_DIR);
    AuthCredentials *creds = Q_NULLPTR;
    CommandsFactory *commandsFactory = Q_NULLPTR;
};

#endif // TESTUPLOADDELETECOMMAND_H
