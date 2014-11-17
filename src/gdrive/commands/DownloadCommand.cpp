#include "DownloadCommand.h"
DownloadCommand::DownloadCommand()
{
}


void DownloadCommand::execute(const QVariantMap& options) {
  // const QString& downloadDir=config()->get("cloud/dbdir").toString(), const QString &dbName="")
  Q_ASSERT(options.size()==3);
  options.contains(OPTION_FI) &&
    const FileInfo fi = options.at(0).value<FileInfo>();
  const QString dbDir = options.at(1).toString();
  const QString dbName = options.at(2).toString();

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

  localDb.length() == 0 ? emitError(
    Errors::DownloadError::DBNAME_ERROR_PROBLEM,
    "Failed to get database name either from parameter or remote title") : void();
  Q_ASSERT(localDb.length() > 0);

  GoogleDriveTools::removeLocalDatabase(localDb) ? void() :
  emitError(
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
  GoogleDrive::Command::NoError ? result.append(localDb) : emitError(
    Errors::DownloadError::GOOGLEDRIVEAPI_DOWNLOAD_COMMAND_PROBLEM,
    cmd.errorString());
}


KeePassxDriveSync::Command* DownloadCommand::newInstance() {
 return new DownloadCommand;
}
