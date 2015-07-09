#include "DownloadCommand.h"
using namespace KeePassxDriveSync;

Q_DECLARE_METATYPE(RemoteFile)

DownloadCommand::DownloadCommand(Session *session) : session(session)
{
}

DownloadCommand::~DownloadCommand() {

}
void DownloadCommand::execute(const QVariantMap options)
{
    qRegisterMetaType<RemoteFile>("RemoteFile");
    Q_ASSERT(options.size() >= 1);
    const RemoteFile rFi = parseOption<RemoteFile>(options, OPTION_FI);
    const QString dbDir
        = parseOption(options, OPTION_DB_DIR, config()->get("cloud/dbdir").toString());
    const QString dbName = parseOption(options, OPTION_DB_NAME, QString(""));

    const FileInfo fi = RemoteFileImpl::toGDriveFileInfo(rFi);
    CommandDownloadFile cmd(session);

    // connect(&cmd,
    // SIGNAL(progress(qint64,
    // qint64)), this,
    // SLOT(downloadProgress(qint64, qint64)));

    QString localDb;

    // take database name from remote title if not specified in params
    dbName.length() == 0 ? localDb = QDir::toNativeSeparators(
        dbDir + "/" + fi.title()) : localDb = QDir::toNativeSeparators(
                                         dbDir + "/" + dbName);

    localDb.length() == 0 ? emitError(
        Errors::DownloadError::DBNAME_ERROR_PROBLEM,
        "Failed to get database name either from parameter or remote title") : void();
    Q_ASSERT(localDb.length() > 0);

    GoogleDriveTools::removeLocalDatabase(localDb) ? void()
    : emitError(
        Errors::DownloadError::COMMON_DOWNLOAD_PROBLEM,
        QString("Failed to remove old database file %1").arg(localDb));

    qDebug() << QString("Downloading cloud db to file::" + localDb);
    QFile f(localDb);
    f.open(QFile::WriteOnly);

    qDebug() << QString("Url::" + fi.downloadUrl().path());
    Q_FOREACH(const QString &key, fi.exportList().keys()) {
        qDebug() << "key = " + fi.exportList().value(key).toString();
    }
    cmd.exec(fi.downloadUrl(), &f);
    cmd.waitForFinish(false);
    f.close();

    if (cmd.error() == GoogleDrive::Command::NoError) {
        setResult(KeePassxDriveSync::ResultBuilder().addValue(localDb).build());
        emitSuccess();
    } else {
        emitError(
            Errors::DownloadError::DOWNLOAD_COMMAND_PROBLEM,
            cmd.errorString());
    }
}

KeePassxDriveSync::Command *DownloadCommand::newInstance(Session *session)
{
    return new DownloadCommand(session);
}
