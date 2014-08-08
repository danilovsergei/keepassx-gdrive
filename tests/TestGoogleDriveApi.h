#ifndef TESTGOOGLEDRIVEAPI_H
#define TESTGOOGLEDRIVEAPI_H
#include <QtCore/QObject>
#include "gdrive/GoogleDriveApi.h"
#include "qtdrive/lib/file_info.h"

/**
 * @brief The TestGoogleDriveApi class - helper which wraps all signals and slots for getDatabases API call into the signal method.
 * Payoff for that is application has to wait until that call will be finished
 */
class TestGoogleDriveApi : public QObject
{
Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testUploadNewDb();
    void testUpdateExistingDb();
    void testGetDatabasesHHelper();

private:
    const QString createLocalDatabase();
    void deleteDb(FileInfo &db);
    void uploadDb(const QString& dbName,const QString& dbPath);
    GoogleDriveApi*   gdrive;
    QString parentDir;
    QString dbName;

};

#endif // TESTGOOGLEDRIVEAPI_H
