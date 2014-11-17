#ifndef DownloadCommand_H
#define DownloadCommand_H
#include "gdrive/commands/Command.h"
#include "qtdrive/lib/file_info.h"
#include "qtdrive/lib/command_download_file.h"
#include <gdrive/GoogleDriveSession.h>
#include <QtCore/QDir>
#include <gdrive/Errors.h>
#include <gdrive/GoogleDriveTools.h>

Q_DECLARE_METATYPE(GoogleDrive::FileInfo)
using namespace KeePassxDriveSync;
class DownloadCommand : public KeePassxDriveSync::Command
{
public:
    DownloadCommand();
    void execute(const QVariantMap &options);
    static Command* newInstance();
};

#endif // DownloadCommand_H
