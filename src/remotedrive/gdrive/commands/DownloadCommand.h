#ifndef DownloadCommand_H
#define DownloadCommand_H

#include "remotedrive/gdrive/commands/Command.h"
#include "qtdrive/lib/file_info.h"
#include "qtdrive/lib/command_download_file.h"
#include <QtCore/QDir>
#include <remotedrive/Errors.h>
#include "remotedrive/gdrive/GoogleDriveTools.h"

Q_DECLARE_METATYPE(GoogleDrive::FileInfo)
using namespace KeePassxDriveSync;
class DownloadCommand : public KeePassxDriveSync::Command
{
public:
    DownloadCommand(Session* session);
    void execute(const QVariantMap &options);
    static Command* newInstance();
private:
    Session* session;
};

#endif // DownloadCommand_H
