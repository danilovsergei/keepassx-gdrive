#ifndef LISTCOMMAND_H
#define LISTCOMMAND_H
#include "remotedrive/Command.h"
#include "qtdrive/lib/command_file_list.h"
#include "remotedrive/gdrive/RemoteFileImpl.h"
#include "remotedrive/gdrive/QueryEntry.h"
#include "core/Config.h"
#include "remotedrive/RemoteFile.h"


using namespace GoogleDrive;
class ListCommand : public KeePassxDriveSync::Command
{
    Q_OBJECT
public:
    ListCommand(Session *session);
    void execute(const QVariantMap options);
    ~ListCommand();
private:
    Session *session;
};
typedef QList<QueryEntry> DbFilter;
#endif // LISTCOMMAND_H
