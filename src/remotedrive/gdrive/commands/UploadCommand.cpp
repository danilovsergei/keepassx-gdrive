#include "UploadCommand.h"

Q_DECLARE_METATYPE(RemoteFileList)
Q_DECLARE_METATYPE(QList<QueryEntry>)

UploadCommand::UploadCommand(GoogleDrive::Session *session)
    :session(session)
{}

void UploadCommand::execute(const QVariantMap &options)
{
  const QString filePath = parseOption<QString>(options, OPTION_ABSOLUTE_DB_NAME);
  const QDateTime lastModified = parseOption<QDateTime>(options, OPTION_LAST_MODIFIED_TIME);
  const QString parent = parseOption<QString>(options, OPTION_PARENT_NAME);

  // urlfields are custom keepassx fields which will be added to the file. Not visible through GoogleDrive UI
  QMap<QString, QString> urlFields;
  // bodyfields are standard GoogleDrive properties and visible by user through UI
  QMap<QString, QString> bodyFields;

  // create initial FileInfo object with parameters to tell GoogleDrive api what we are uploading
  qRegisterMetaType<FileInfo>("FileInfo");
  QString dbName = QDir::toNativeSeparators(QFileInfo(filePath).fileName());
  FileInfo fi;
  fi.setTitle(dbName);

  // Keepass db folder is set to root in Google drive if user didn't not customize it
  if (parent.length() > 0) fi.setParents(QStringList(parent));
  else if (config()->get("cloud/GdriveKeepassFolder").toString().length() > 0)
    fi.setParents(QStringList(config()->get("cloud/GdriveKeepassFolder").toString()));

  qDebug() << "Start uploading the database " + filePath;
  CommandUploadFile *cmd = new CommandUploadFile(session);
  cmd->setAutoDelete(true);

  qRegisterMetaType<GoogleDrive::FileInfoList>("GoogleDrive::FileInfoList");
  QList<QueryEntry> filter;
  filter.append(GoogleDriveTools::getDbNameFilter(filePath));
  QFile file(filePath);

  // Always check if cloud db exists to prevent uploading second db with a same name
  ListCommand listCommand(session);
  listCommand.execute(OptionsBuilder().addOption(OPTION_DB_FILTER,
                                                 GoogleDriveTools::getDbNameFilter(filePath)).build());
  RemoteFileList dbList = listCommand.getResult().at(0).value<RemoteFileList>();
  Q_ASSERT(dbList.size() <= 1);

  if (dbList.size() == 0)
    cmd->exec(fi, &file);
  else
    cmd->exec(RemoteFileImpl::toGDriveFileInfo(dbList.first()), &file, urlFields, bodyFields);

  if (!cmd->waitForFinish(false)) {
    emitError(Errors::FileError::UPLOAD_FILES_PROBLEM,
              QString("Failed to upload file %1").arg(dbName));
    return;
  }

  if (cmd->error() != Errors::NO_ERROR) {
    emitError(Errors::FileError::UPLOAD_FILES_PROBLEM,
              QString("Failed to upload file %1").arg(dbName));
    return;
  }

  if (cmd->resultFileInfo().isEmpty()) {
    emitError(Errors::FileError::UPLOAD_FILES_PROBLEM,
              QString("Failed to upload file %1").arg(dbName));
    return;
  }

  FileInfo uploadedDb = cmd->resultFileInfo();

  // use extra query to update lastmodified date only for newly uploaded databases
  if (dbList.size() == 0) setLastModificationDate(uploadedDb, lastModified);
  emitSuccess();
}

void UploadCommand::setLastModificationDate(const FileInfo &fi, const QDateTime &lastModified)
{
  CommandUpdate *cmd = new CommandUpdate(session);

  QMap<QString, QString> urlProperties;
  QMap<QString, QString> bodyProperties;
  urlProperties["newRevision"] = "false";
  urlProperties["setModifiedDate"] = "true";
  bodyProperties["modifiedDate"] = GoogleDriveTools::toRFC3339(lastModified);
  cmd->exec(fi, urlProperties, bodyProperties);

  if (!cmd->waitForFinish(true)) {
    emitError(Errors::FileError::SET_CUSTOM_PROPERTY_PROBLEM,
              QString("Failed set last modification date custom property to %1").arg(lastModified.
                                                                                     toString()));
    return;
  }
}

UploadCommand::~UploadCommand()
{
}