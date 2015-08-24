#ifndef DownloadCommand_H
#define DownloadCommand_H

#include "remotedrive/RemoteCommand.h"
#include "qtdrive/lib/file_info.h"
#include "qtdrive/lib/command_download_file.h"
#include <QtCore/QDir>
#include <remotedrive/Errors.h>
#include "remotedrive/gdrive/GoogleDriveTools.h"
#include "qtdrive/lib/session.h"
#include "remotedrive/RemoteFile.h"
#include "remotedrive/gdrive/RemoteFileImpl.h"
#include <QtCore/QUrl>
#include <remotedrive/gdrive/BaseCommand.h>
#include <remotedrive/AuthCredentials.h>
#include <remotedrive/RemoteTools.h>

Q_DECLARE_METATYPE(GoogleDrive::FileInfo)
using namespace KeePassxDriveSync;
using namespace GoogleDrive;

class DownloadCommand : public BaseCommand
{
  Q_OBJECT
public:
  DownloadCommand(AuthCredentials *creds);
  ~DownloadCommand();
  void execute(const QVariantMap options);
};

#endif // DownloadCommand_H
