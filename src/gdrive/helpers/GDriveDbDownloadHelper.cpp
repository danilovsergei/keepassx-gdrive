#include "GDriveDbDownloadHelper.h"
GDriveDbDownloadHelper::GDriveDbDownloadHelper()
{}

GDriveDbDownloadHelper::~GDriveDbDownloadHelper() {}

void GDriveDbDownloadHelper::downloadDatabaseParallel(const FileInfo& fi,
                                                      const QString & downloadDir,
                                                      const QString & dbName) {
  QtConcurrent::run(this,
                    &GDriveDbDownloadHelper::downloadDatabase,
                    fi,
                    downloadDir, dbName);
}

QString GDriveDbDownloadHelper::downloadDatabase(const FileInfo& fi,
                                                 const QString & dbDir,
                                                 const QString & dbName) {
  CommandDownloadFile cmd(getSession());

  connect(&cmd,
          SIGNAL(progress(qint64,
                          qint64)), this,
          SLOT(downloadProgress(qint64, qint64)));

  QString localDb;

  // take database name from remote title if not specified in params
  dbName.length() == 0 ? localDb = QDir::toNativeSeparators(
    dbDir + "/" + fi.title()) : localDb = QDir::toNativeSeparators(
                                     dbDir + "/" + dbName);

  localDb.length() == 0 ? emitDownloadDatabaseError(
    Errors::DownloadError::DBNAME_ERROR_PROBLEM,
    "Failed to get database name either from parameter or remote title") : void();
  Q_ASSERT(localDb.length() > 0);

  GoogleDriveTools::removeLocalDatabase(localDb) ? void() :
  emitDownloadDatabaseError(
    Errors::DownloadError::COMMON_DOWNLOAD_PROBLEM,
    QString("Failed to remove old database file %1").arg(localDb));

  qDebug() << QString("Downloading cloud db to file::" + localDb);
  QFile f(localDb);
  f.open(QFile::WriteOnly);

  qDebug() << QString("Url::" + fi.downloadUrl().path());
  Q_FOREACH(const QString &key, fi.exportList().keys()) {
    qDebug() << fi.exportList().value(key).toString();
  }
  cmd.exec(fi.downloadUrl(), &f);
  cmd.waitForFinish(false);
  f.close();
  cmd.error() ==
  Command::NoError ? emitDownloadDatabaseDone(localDb) : emitDownloadDatabaseError(
    Errors::DownloadError::GOOGLEDRIVEAPI_DOWNLOAD_COMMAND_PROBLEM,
    cmd.errorString());



  return localDb;
}

void GDriveDbDownloadHelper::emitDownloadDatabaseError(int            errorType,
                                                       const QString& description)
{
  errorCode    = errorType;
  errorMessage = description;
  Q_EMIT downloadDatabaseError(errorType, description);
    qDebug() << "Emit the error===="+description;
}

void GDriveDbDownloadHelper::emitDownloadDatabaseDone(QString localDb) {
  Q_EMIT downloadDatabaseDone(localDb);
}

void GDriveDbDownloadHelper::downloadProgress(qint64 v, qint64 total)
{
    qDebug() << QString("Download progress " + QString::number(
                        v) + " from " + QString::number(total));
}

QSharedPointer<GDriveDbDownloadHelper>GDriveDbDownloadHelper::newInstance() {
  return QSharedPointer<GDriveDbDownloadHelper>(new GDriveDbDownloadHelper());
}

int GDriveDbDownloadHelper::getErrorCode() {
  return errorCode;
}

const QString GDriveDbDownloadHelper::getErrorMessage() {
  return errorMessage;
}
