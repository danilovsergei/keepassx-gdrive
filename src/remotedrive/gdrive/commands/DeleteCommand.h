#ifndef DELETECOMMAND_H
#define DELETECOMMAND_H
#include <remotedrive/Command.h>
#include <qtdrive/lib/session.h>
#include "remotedrive/gdrive/GDriveConstants.h"
#include <remotedrive/RemoteFile.h>
#include "qtdrive/lib/command_delete.h"
#include <remotedrive/gdrive/commands/ListCommand.h>
#include <remotedrive/gdrive/GoogleDriveTools.h>
#include <remotedrive/OptionsBuilder.h>
#include <remotedrive/AuthCredentials.h>
#include <remotedrive/gdrive/BaseCommand.h>

using namespace KeePassxDriveSync;
using namespace GoogleDrive;

class DeleteCommand : public BaseCommand
{
Q_OBJECT
public:
    DeleteCommand(AuthCredentials* creds);
    void execute(const QVariantMap options);
    ~DeleteCommand();
};

#endif // DELETECOMMAND_H
