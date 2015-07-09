#include "DeleteCommand.h"
Q_DECLARE_METATYPE(QList<QueryEntry>)
Q_DECLARE_METATYPE(RemoteFile)
Q_DECLARE_METATYPE(RemoteFileList)

DeleteCommand::DeleteCommand(Session *session) :
  session(session)
{
}

void DeleteCommand::execute(const QVariantMap options)
{
  Q_ASSERT(options.size() == 1);

  RemoteFile db = parseOption<RemoteFile>(options, OPTION_REMOTE_FILE);

  CommandDelete *deleteCmd = new CommandDelete(session);

  deleteCmd->exec(db.getId());

  if (!deleteCmd->waitForFinish(false)) {
    emitError(
      Errors::FileError::DELETE_DB_PROBLEM,
      QString("Failed to wait while deleting database from the cloud %1").arg(db.getTitle()));
    return;
  }

  ListCommand listCommand(session);
  const QList<QueryEntry> dbFilter= GoogleDriveTools::getDbNameFilter(db.getTitle());
  QVariantMap deleteOptions = OptionsBuilder().addOption(OPTION_DB_FILTER, dbFilter).build();

  listCommand.execute(deleteOptions);
  const RemoteFileList dbList = listCommand.getResult().at(0).value<RemoteFileList>();

  Q_FOREACH(RemoteFile dbRec, dbList) {
    // TODO it looks like we need to make sure no record with deleted id present.
    // probably there is a targeted request for it exist
     if (dbRec.getId() != db.getId()) {
      emitError(
        Errors::FileError::DELETE_DB_PROBLEM,
        QString("Failed to delete database. %1 still exists at Google Drive").arg(db.getTitle()));
      return;
    }
  }
  emitSuccess();
}

DeleteCommand::~DeleteCommand()
{
}
