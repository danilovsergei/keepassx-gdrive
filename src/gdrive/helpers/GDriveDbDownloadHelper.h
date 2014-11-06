#ifndef GDRIVEDBDOWNLOADHELPER_H
#define GDRIVEDBDOWNLOADHELPER_H
#include "../GDriveSyncObject.h"
#include "../../qtdrive/lib/file_info.h"
#include "../../qtdrive/lib/command_download_file.h"
#include <QtCore/QDebug>
#include <QtCore/QtConcurrentRun>
#include <QtCore/QUrl>
#include "../GoogleDriveSession.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include "../../core/Config.h"
#include "../Errors.h"
#include "../GoogleDriveTools.h"

using namespace GoogleDrive;
using namespace DatabaseSync;
class GDriveDbDownloadHelper: public QObject

{
    Q_OBJECT
public:
    static QSharedPointer<GDriveDbDownloadHelper> newInstance();
    ~GDriveDbDownloadHelper();
    /**
     * @brief downloadDatabase downloads remote google drive database locally
     * @param fi FileInfo object of remote database
     * @param downloadDir directory where to download database. Taken from config if no specified
     * @param dbName how downloaded database will be called. Taken from FileInfo.title() if not specified
     * @return path to downloaded local database. Always check getErrorCode() if there are any errors happen
     */
    QString downloadDatabase(const FileInfo& fi,const QString& downloadDir=config()->get("cloud/dbdir").toString(), const QString &dbName="");
    void downloadDatabaseParallel(const FileInfo& fi,const QString& downloadDir=config()->get("cloud/dbdir").toString(), const QString &dbName="");
    int getErrorCode();
    const QString getErrorMessage();

private:
    GDriveDbDownloadHelper();
    void emitDownloadDatabaseError(int errorType,const QString& description);
    void emitDownloadDatabaseDone(QString localDb);
    void downloadProgress(qint64 v, qint64 total);
    QString errorMessage = "";
    int errorCode = Errors::NO_ERROR;
Q_SIGNALS:
    void downloadDatabaseError(int ErrorType,const QString& description);
    void downloadDatabaseDone(const QString&);

};

#endif // GDRIVEDBDOWNLOADHELPER_H
