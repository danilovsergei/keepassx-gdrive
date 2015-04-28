#ifndef DownloadCommand_H
#define DownloadCommand_H

#include "remotedrive/gdrive/commands/Command.h"
#include "qtdrive/lib/file_info.h"
#include "qtdrive/lib/command_download_file.h"
#include <QtCore/QDir>
#include <remotedrive/Errors.h>
#include "remotedrive/gdrive/GoogleDriveTools.h"
#include "qtdrive/lib/session.h"
#include "remotedrive/RemoteFile.h"
#include "remotedrive/gdrive/RemoteFileImpl.h"
#include <QtCore/QUrl>

Q_DECLARE_METATYPE(GoogleDrive::FileInfo)
using namespace KeePassxDriveSync;
using namespace GoogleDrive;
class DownloadCommand : public KeePassxDriveSync::Command
{
public:
    DownloadCommand(Session *session);
    void execute(const QVariantMap &options);
    static KeePassxDriveSync::Command *newInstance(Session *session);
private:
    Session *session;
};

#endif // DownloadCommand_H
