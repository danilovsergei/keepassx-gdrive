#ifndef GOOGLEDRIVEAPI_H
#define GOOGLEDRIVEAPI_H

#include <QtCore/QObject>
#include "GoogleDriveSession.h"
#include "../qtdrive/lib/command_file_list.h"
#include "../qtdrive/lib/command_download_file.h"
#include "../qtdrive/lib/command_upload_file.h"
#include "../qtdrive/lib/command_set_properties.h"
#include "../qtdrive/lib/command_get_properties.h"
#include "../core/Config.h"
#include <QtCore/QDir>
#include <QtCore/QUrl>
class GoogleDriveApi : public QObject
{
    Q_OBJECT
public:
    explicit GoogleDriveApi(QObject *parent = 0);
    ~GoogleDriveApi();
    void  getDatabases();
    void downloadDatabase(const FileInfo& fi);
    void uploadDatabase(const QString& filePath, const QDateTime& lastModified);
    QDateTime getLastModificationDate(const FileInfo& fi);
    static GoogleDriveApi* instance();

private:
    void getDatabasesTh();
    void downloadDatabaseTh(const FileInfo &fi);
    void uploadDatabaseTh(const QString& filePath, const QDateTime &lastModified);
    void setLastModificationDateTh(const FileInfo &fi, const QDateTime &lastModified);
    QDateTime getLastModificationDateTh(const FileInfo& fi);
    static GoogleDriveApi* m_instance;
    Q_DISABLE_COPY(GoogleDriveApi)


Q_SIGNALS:
    void DbListLoaded(GoogleDrive::FileInfoList db_files);
    void downloadDatabaseFinished(const QString& localDb);
    void uploadDatabaseFinished();

public Q_SLOTS:
    void downloadProgress(qint64 v, qint64 total);
    void checkUploadDatabaseStatus();
    void uploadDatabaseProgress(qint64 v, qint64 total);
    void setLastModificationDate(const FileInfo& fi,const QDateTime &lastModified);
};

inline GoogleDriveApi* googleDriveApi(){
    return GoogleDriveApi::instance();
}

#endif // GOOGLEDRIVEAPI_H
