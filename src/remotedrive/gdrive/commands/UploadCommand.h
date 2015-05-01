#ifndef UPLOADCOMMAND_H
#define UPLOADCOMMAND_H

#include <remotedrive/Command.h>
#include "qtdrive/lib/session.h"
#include <remotedrive/gdrive/GDriveConstants.h>
class UploadCommand : public KeePassxDriveSync::Command
{
  Q_OBJECT

public:
  UploadCommand(Session *session);
  /**
   * @brief uploads local database to the google  drive.
   *  This method will always create new remote file. Even if file with such name already exists
   *  QVariantMap options description:
   * @param filePath - path to local database
   * @param lastModified - time when local database was modified last time
   * @param parent - parent directory id in google drive.
   */
  void execute(const QVariantMap &options);
  ~UploadCommand();
private:
  Session *session;
};

#endif // UPLOADCOMMAND_H
