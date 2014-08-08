#include "GoogleDriveApi.h"
#include "QtCore/QFuture"
#include "QtCore/QtConcurrentRun"
#include "QtCore/QFile"
#include <QtCore/QBuffer>
#include <QtCore/QVariant>
#include <QtGui/QErrorMessage>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include "GoogleDriveTools.h"

Q_DECLARE_METATYPE(GoogleDrive::FileInfo)
Q_DECLARE_METATYPE(GoogleDrive::FileInfoList)

GoogleDriveApi * GoogleDriveApi::m_instance(Q_NULLPTR);

GoogleDriveApi::GoogleDriveApi(QObject *parent) :
  QObject(parent)
{}

const FileInfoList GoogleDriveApi::getDatabasesSeq(
  const QList<QueryEntry>& queryFilter) {
  const FileInfoList dbList = getDatabasesTh(queryFilter);
  return dbList;
}

void GoogleDriveApi::getDatabasesPar(const QList<QueryEntry>& queryFilter) {
  qRegisterMetaType<GoogleDrive::FileInfoList>("GoogleDrive::FileInfoList");
  QFuture<GoogleDrive::FileInfoList> future = QtConcurrent::run(this,
                                                                &GoogleDriveApi::getDatabasesTh,
                                                                queryFilter);

  // this method will wait until concurrent thread will be finished and will
  // block the executed of current method
  // const FileInfoList dbList=future.result();
}

FileInfoList GoogleDriveApi::getDatabasesTh(const QList<QueryEntry>& queryFilter)
{
  CommandFileList cmd(getSession());

  cmd.setAutoDelete(true);
  cmd.setFields("items(fileSize,id,title,modifiedDate,description,downloadUrl)");
  QString query = QueryEntry::getEntries(queryFilter);

  // Keepass db folder is set to root in Google drive if use did not customize
  // it
  if (config()->get("cloud/GdriveKeepassFolder").toString().length() >
      0) cmd.execForFolder(config()->get(
                             "cloud/GdriveKeepassFolder").toString(), query);
  else cmd.exec(query);

  if (!cmd.waitForFinish(false)) {
    // TODO move error handling to the main thread
    // QErrorMessage error;
    // error.showMessage("Failed to get databases list"+cmd.errorString());
    // error.exec();
    Q_ASSERT(false);
  }

  qRegisterMetaType<GoogleDrive::FileInfoList>("GoogleDrive::FileInfoList");
  qRegisterMetaType<GoogleDrive::FileInfo>(    "GoogleDrive::FileInfo");
  GoogleDrive::FileInfoList dbList = cmd.files();
  Q_EMIT googleDriveApi()->dbListLoaded(dbList);
  return dbList;
}

const QString GoogleDriveApi::downloadDatabaseSeq(const FileInfo& fi,
                                                  const QString & downloadDir, const QString& dbName) {
  const QString localDb = downloadDatabaseTh(fi, downloadDir,dbName);

  return localDb;
}

void GoogleDriveApi::downloadDatabase(const FileInfo& fi,
                                      const QString & downloadDir, const QString& dbName) {
  QFuture<void> future = QtConcurrent::run(this,
                                           &GoogleDriveApi::downloadDatabaseTh,
                                           fi,
                                           downloadDir,dbName);
}

QString GoogleDriveApi::downloadDatabaseTh(const FileInfo& fi,
                                           const QString & dbDir,const QString& dbName) {
  qDebug() << QString("Url::" + fi.downloadUrl().toString());
  qDebug() << QString("id=" + fi.id());

  CommandDownloadFile cmd(getSession());

  connect(&cmd,
          SIGNAL(progress(qint64,
                          qint64)), googleDriveApi(),
          SLOT(downloadProgress(qint64, qint64)));

  // TODO check if file exists and try to delete it.
  // emit error if delition does not work
  QString localDb;
  if (dbName.length()==0)
  localDb = QDir::toNativeSeparators(dbDir + "/" + fi.title());
  else
  localDb = QDir::toNativeSeparators(dbDir + "/" + dbName);
  Q_ASSERT(localDb.length()>0);
  qDebug() << QString("Writing to file::" + localDb);
  QFile f(localDb);
  f.open(QFile::WriteOnly);

  qDebug() << QString("Url::" + fi.downloadUrl().path());
  Q_FOREACH(const QString &key, fi.exportList().keys()) {
    qDebug() << fi.exportList().value(key).toString();
  }

  cmd.exec(fi.downloadUrl(), &f);

  if (!cmd.waitForFinish(false)) return "";

    qDebug() << "Before Signal emitting;;;";
  Q_EMIT googleDriveApi()->downloadDatabaseFinished(localDb);
    qDebug() << "finished";
  return localDb;
}

/**
 * @brief GoogleDriveApi::uploadDatabase - uploads local database to the google
 * drive. This method will always create new remote file. Even if file with such
 * name already exists
 * @param filePath - path to local database
 * @param lastModified - time when local database was modified last time
 * @param waitForFinish - should we wait until this function will be finished
 * @param parent - parent directory id in google drive.
 */
void GoogleDriveApi::uploadDatabase(const QString  & filePath,
                                    const QDateTime& lastModified,
                                    bool             waitForFinish,
                                    QString          parent) {
  qRegisterMetaType<FileInfo>("FileInfo");
  QString  dbName = QDir::toNativeSeparators(QFileInfo(filePath).fileName());
  FileInfo fi;
  QMap<QString, QString> urlFields;
  QMap<QString, QString> bodyFields;
  fi.setTitle(dbName);

  // Keepass db folder is set to root in Google drive if use did not customize
  // it
  if (parent.length() > 0) fi.setParents(QStringList(parent));

  else if (config()->get("cloud/GdriveKeepassFolder").toString().length() >
           0) fi.setParents(QStringList(config()->get(
                                          "cloud/GdriveKeepassFolder").toString()));



  if (waitForFinish) uploadDatabaseTh(filePath,
                                      fi,
                                      urlFields,
                                      bodyFields,
                                      lastModified);
  else QFuture<void>future = QtConcurrent::run(this,
                                               &GoogleDriveApi::uploadDatabaseTh,
                                               filePath,
                                               fi,
                                               urlFields,
                                               bodyFields,
                                               lastModified);
}

/**
 * @brief GoogleDriveApi::updateDatabase - updates existing in the cloud
 * database with a new revision.
 * @param filePath - path to local database
 * @param db - remote database object in google drive , which will be updated
 * with a new revision
 * @param lastModified -
 * @param waitForFinish
 */
void GoogleDriveApi::updateDatabase(const QString  & filePath,
                                    const FileInfo & db,
                                    const QDateTime& lastModified,
                                    bool             waitForFinish) {
  QMap<QString, QString> urlFields;
  QMap<QString, QString> bodyFields;
  urlFields["setModifiedDate"] = "true";
  bodyFields["modifiedDate"]   = GoogleDriveTools::toRFC3339(lastModified);

  if (waitForFinish) uploadDatabaseTh(filePath,
                                      db,
                                      urlFields,
                                      bodyFields,
                                      lastModified);
  else QFuture<void>future = QtConcurrent::run(this,
                                               &GoogleDriveApi::uploadDatabaseTh,
                                               filePath,
                                               db,
                                               urlFields,
                                               bodyFields,
                                               lastModified);
}

void GoogleDriveApi::updateDatabase(const QString& filePath,
                                    const FileInfo& db,
                                    const QMap<QString, QString>& urlFields,
                                    const QMap<QString, QString>& bodyFields,
                                    const QDateTime& lastModified,
                                    bool waitForFinish) {
  if (waitForFinish) uploadDatabaseTh(filePath,
                                      db,
                                      urlFields,
                                      bodyFields,
                                      lastModified);
  else QFuture<void>future = QtConcurrent::run(this,
                                               &GoogleDriveApi::uploadDatabaseTh,
                                               filePath,
                                               db,
                                               urlFields,
                                               bodyFields,
                                               lastModified);
}

void GoogleDriveApi::setLastModificationDate(const FileInfo & fi,
                                             const QDateTime& lastModified,
                                             bool             waitForFinish) {
  if (!waitForFinish) QFuture<void>future = QtConcurrent::run(this,
                                                              &GoogleDriveApi::setLastModificationDateTh,
                                                              fi,
                                                              lastModified);
  else setLastModificationDateTh(fi, lastModified);
}

QDateTime GoogleDriveApi::getLastModificationDate(const FileInfo& fi) {
  QFuture<QDateTime> lastModificationDate = QtConcurrent::run(this,
                                                              &GoogleDriveApi::getLastModificationDateTh,
                                                              fi);
  qDebug() << "grabbed time here::" + lastModificationDate.result().toString();
  return lastModificationDate;
}

void GoogleDriveApi::uploadDatabaseTh(const QString& filePath,
                                      const FileInfo& fi,
                                      const QMap<QString, QString>& urlFields,
                                      const QMap<QString, QString>& bodyFields,
                                      const QDateTime& lastModified) {
  qDebug() << "Start uploading the database " + filePath;

  // CommandUploadFile cmd(getSession());
  CommandUploadFile *cmd = new CommandUploadFile(getSession());
  cmd->setAutoDelete(true);
  connect(cmd, SIGNAL(finished()), this,
          SLOT(checkUploadDatabaseStatus()));
  connect(cmd,
          SIGNAL(progress(qint64,
                          qint64)), this,
          SLOT(uploadDatabaseProgress(qint64, qint64)));

  qRegisterMetaType<GoogleDrive::FileInfoList>("GoogleDrive::FileInfoList");
  QList<QueryEntry> filter;
  QueryEntry filterEntry(QueryEntry::QueryFilter::DBNAME, QStringList(QFileInfo(
                                                                        filePath).fileName()));
  filter.append(filterEntry);
  QFile file(filePath);
  FileInfoList dbList;

  // It's a default behavior to always check if cloud db exists to prevent
  // uploading second db with a same name
  if (DebugGoogleDriveApi::debugOptions[DebugGoogleDriveApi::
                                        DEBUG_CHECK_FOR_EXISTING_CLOUD_DB] !=
      "0") {
    dbList = getDatabasesTh(filter);
    Q_ASSERT(dbList.size() <= 1);

    if (dbList.size() == 0)
    {
      cmd->exec(fi, &file);
    }
    else {
      cmd->exec(dbList.first(), &file, urlFields, bodyFields);
    }
  }

  // if set db will be uploaded without checking with a cloud
  // which potentially could lead 2 db with a same names will coexist in the
  // cloud
  else {
    qDebug() <<
      "DEBUG_CHECK_FOR_EXISTING_CLOUD_DB set to disable db existance check in the cloud while uploading";
    cmd->exec(fi, &file);
  }


  if (!cmd->waitForFinish(false)) {
    return;
  }

  // resultFileInfo isEmpty should return false if upload db was finished
  // successfully
  FileInfo uploadedDb = cmd->resultFileInfo();

  // use extra query to update lastmodified date only for newly uploaded
  // databases
  if (dbList.size() == 0) setLastModificationDateTh(uploadedDb, lastModified);
}

// void GoogleDriveApi::setLastModificationDateTh(const FileInfo& fi,const
// QDateTime &lastModified) {
// Command* cmd = new CommandSetFileProperties(getSession());

// QMap<QString,QString> properties;
// properties[LAST_MODIFIED]=GoogleDriveTools::toRFC3339(lastModified);
// cmd->exec(fi,properties);

// if (!cmd->waitForFinish(true))
//    return;
// }

void GoogleDriveApi::setLastModificationDateTh(const FileInfo & fi,
                                               const QDateTime& lastModified) {
  CommandUpdate *cmd = new CommandUpdate(getSession());

  QMap<QString, QString> urlProperties;
  QMap<QString, QString> bodyProperties;
  urlProperties["newRevision"]     = "false";
  urlProperties["setModifiedDate"] = "true";
  bodyProperties["modifiedDate"]   = GoogleDriveTools::toRFC3339(lastModified);
  cmd->exec(fi, urlProperties, bodyProperties);

  if (!cmd->waitForFinish(true)) return;
}

QDateTime GoogleDriveApi::getLastModificationDateTh(const FileInfo& fi) {
  CommandGetFileProperties *cmd = new CommandGetFileProperties(getSession());

  cmd->exec(fi);

  if (!cmd->waitForFinish(true)) return QDateTime();

  QMap<QString, QString> props = cmd->getProperties();
  QDateTime time;

  if (props.contains(LAST_MODIFIED)) {
    time = QDateTime::fromString(props[LAST_MODIFIED]);
  }

  // return null in case if LAST_MODIFIED property does not exist
  return time;
}

void GoogleDriveApi::checkUploadDatabaseStatus()
{
  CommandUploadFile *cmd = qobject_cast<CommandUploadFile *>(sender());

  if (cmd->error() == Command::NoError) {
    qDebug() << "Uploading finished successfully: " + cmd->resultFileInfo().id();
  }

  else {
    qDebug() << cmd->errorString();
  }
  Q_EMIT googleDriveApi()->uploadDatabaseFinished();
}

void GoogleDriveApi::uploadDatabaseProgress(qint64 v, qint64 total)
{
    qDebug() << "Upload progress: " + QString::number(v) + " from " +
    QString::number(total);
}

void GoogleDriveApi::downloadProgress(qint64 v, qint64 total)
{
    qDebug() << QString("Download progress " + QString::number(
                        v) + " from " + QString::number(total));
}

GoogleDriveApi * GoogleDriveApi::instance() {
  if (!m_instance) {
    m_instance = new GoogleDriveApi();
  }
  return m_instance;
}

GoogleDriveApi::~GoogleDriveApi() {}

void DebugGoogleDriveApi::setDebugOption(short key, QString value) {
  DebugGoogleDriveApi::debugOptions[key] = value;
}

void DebugGoogleDriveApi::clearDebugOptions() {
  DebugGoogleDriveApi::debugOptions = QMap<short, QString>();
}
