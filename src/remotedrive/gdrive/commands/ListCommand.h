#ifndef LISTCOMMAND_H
#define LISTCOMMAND_H
#include "remotedrive/Command.h"
#include "qtdrive/lib/command_file_list.h"
#include "remotedrive/gdrive/RemoteFileImpl.h"
#include "remotedrive/gdrive/QueryEntry.h"
#include "core/Config.h"
#include "remotedrive/RemoteFile.h"
#include <remotedrive/AuthCredentials.h>
#include <remotedrive/gdrive/BaseCommand.h>

using namespace GoogleDrive;
class ListCommand : public BaseCommand
{
    Q_OBJECT
public:
    ListCommand(AuthCredentials *creds);
    void execute(const QVariantMap options);
    ~ListCommand();
};
typedef QList<QueryEntry> DbFilter;
#endif // LISTCOMMAND_H
