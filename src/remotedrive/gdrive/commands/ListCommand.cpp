#include "ListCommand.h"

Q_DECLARE_METATYPE(DbFilter)
Q_DECLARE_METATYPE(RemoteFileList)

ListCommand::ListCommand(AuthCredentials *creds)
    : BaseCommand(creds)
{
}

ListCommand::~ListCommand() {

}

void ListCommand::execute(const QVariantMap options)
{
    DbFilter queryFilter = options.value(OPTION_DB_FILTER).value<DbFilter>();

    CommandFileList cmd(getSession());
    cmd.setAutoDelete(true);
    cmd.setFields("items(fileSize,id,title,modifiedDate,description,downloadUrl)");
    QString query = QueryEntry::getEntries(queryFilter);

    // Keepass db folder is set to root in Google drive if user did not customize  it
    if (config()->get("cloud/GdriveKeepassFolder").toString().length()
        > 0) cmd.execForFolder(config()->get(
                                   "cloud/GdriveKeepassFolder").toString(), query);
    else cmd.exec(query);
    cmd.waitForFinish(false);
    if (cmd.error() == Errors::NO_ERROR) {
        qRegisterMetaType<GoogleDrive::FileInfoList>("GoogleDrive::FileInfoList");
        qRegisterMetaType<GoogleDrive::FileInfo>("GoogleDrive::FileInfo");
        GoogleDrive::FileInfoList dbList = cmd.files();
        RemoteFileList remoteFileList = RemoteFileImpl::fromGDriveFileInfoList(dbList);
        setResult(KeePassxDriveSync::ResultBuilder().addValue(remoteFileList).build());
        emitSuccess();
    } else {
        emitError(Errors::FileError::LIST_FILES_PROBLEM,
            QString("Failed to list files for keepass directory"));
    }
}
