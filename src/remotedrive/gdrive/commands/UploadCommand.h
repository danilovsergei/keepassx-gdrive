#ifndef UPLOADCOMMAND_H
#define UPLOADCOMMAND_H

#include <remotedrive/RemoteCommand.h>
#include "qtdrive/lib/session.h"
#include <remotedrive/gdrive/GDriveConstants.h>
#include <QtCore/QDateTime>
#include <core/Database.h>
#include <QtCore/QDebug>

#include <qtdrive/lib/command_upload_file.h>
#include <qtdrive/lib/command_update.h>

#include  "remotedrive/gdrive/QueryEntry.h"
#include "qtdrive/lib/file_info.h"
#include "remotedrive/gdrive/GoogleDriveTools.h"
#include "remotedrive/gdrive/commands/ListCommand.h"
#include  <remotedrive/RemoteFile.h>
#include "remotedrive/OptionsBuilder.h"
#include <remotedrive/gdrive/CommandsFactoryImpl.h>
#include <remotedrive/gdrive/BaseCommand.h>

using namespace GoogleDrive;
class UploadCommand : public BaseCommand
{
  Q_OBJECT

public:
  UploadCommand(AuthCredentials *creds);
  /**
   * @brief uploads local database to the google  drive.
   *  This method will always create new remote file. Even if file with such name already exists
   *  QVariantMap options description:
   * @param filePath - path to local database
   * @param lastModified - time when local database was modified last time
   * @param parent - parent directory id in google drive.
   */
  void execute(const QVariantMap options);
  ~UploadCommand();
private:
  void setLastModificationDate(const FileInfo & fi, const QDateTime& lastModified);
};

#endif // UPLOADCOMMAND_H
