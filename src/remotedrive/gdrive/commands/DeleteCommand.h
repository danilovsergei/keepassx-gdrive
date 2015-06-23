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

using namespace KeePassxDriveSync;
using namespace GoogleDrive;

class DeleteCommand : public KeePassxDriveSync::Command
{
Q_OBJECT
public:
    DeleteCommand(Session *session);
    void execute(const QVariantMap options);
    ~DeleteCommand();
private:
    Session *session;
};

#endif // DELETECOMMAND_H
