#ifndef GOOGLEDRIVEAPI_H
#define GOOGLEDRIVEAPI_H

#include <QtCore/QObject>
#include "GoogleDriveSession.h"
#include "../qtdrive/lib/command_file_list.h"
#include "../qtdrive/lib/command_download_file.h"
#include "../qtdrive/lib/command_upload_file.h"
#include "../qtdrive/lib/command_set_properties.h"
#include "../qtdrive/lib/command_get_properties.h"
#include "../qtdrive/lib/command_update.h"
#include "../core/Config.h"
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include "QueryEntry.h"
#include <QtCore/QMap>
#include <QtCore/QList>
#include "GDriveConstants.h"
class GoogleDriveApi : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(GoogleDriveApi)
public:
    explicit GoogleDriveApi(QObject *parent = 0);
    ~GoogleDriveApi();
    void getDatabasesPar(const QList<QueryEntry>& queryFilter = QList<QueryEntry>());
    const FileInfoList getDatabasesSeq(const QList<QueryEntry>& queryFilter);
    void downloadDatabase(const FileInfo& fi, const QString& downloadDir=config()->get("cloud/dbdir").toString(), const QString &dbName="");
    const QString downloadDatabaseSeq(const FileInfo& fi, const QString& downloadDir=config()->get("cloud/dbdir").toString(),const QString &dbName="");
    void uploadDatabase(const QString& filePath, const QDateTime& lastModified, bool waitForFinish=false, QString parent="");
    void updateDatabase(const QString& filePath, const FileInfo& db, const QDateTime &lastModified, bool waitForFinish);
    void updateDatabase(const QString& filePath, const FileInfo& db, const QMap<QString,QString>& urlFields,const QMap<QString,QString>& bodyFields,const QDateTime &lastModified, bool waitForFinish);
    QDateTime getLastModificationDate(const FileInfo& fi);
    static GoogleDriveApi* instance();


private:
    FileInfoList getDatabasesTh(const QList<QueryEntry>& queryFilter);
    QString downloadDatabaseTh(const FileInfo& fi, const QString& dbDir, const QString &dbName);
    void uploadDatabaseTh(const QString& filePath, const FileInfo& fi, const QMap<QString, QString>& fields, const QMap<QString, QString>& bodyFields, const QDateTime &lastModified);
    void setLastModificationDateTh(const FileInfo &fi, const QDateTime &lastModified);
    QDateTime getLastModificationDateTh(const FileInfo& fi);
    static GoogleDriveApi* m_instance;




Q_SIGNALS:
    void dbListLoaded(GoogleDrive::FileInfoList db_files);
    void downloadDatabaseFinished(QString localDb);
    void uploadDatabaseFinished();

public Q_SLOTS:
    void downloadProgress(qint64 v, qint64 total);
    void checkUploadDatabaseStatus();
    void uploadDatabaseProgress(qint64 v, qint64 total);
    void setLastModificationDate(const FileInfo& fi, const QDateTime &lastModified, bool waitForFinish=false);
};

inline GoogleDriveApi* googleDriveApi(){
    return GoogleDriveApi::instance();
}

namespace DebugGoogleDriveApi {
const short DEBUG_CHECK_FOR_EXISTING_CLOUD_DB=0;
void setDebugOption(short key, QString value);
static QMap<short,QString> debugOptions;
void clearDebugOptions();
}



#endif // GOOGLEDRIVEAPI_H
