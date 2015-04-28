#include "GoogleDriveTools.h"

QString GoogleDriveTools::toRFC3339(const QDateTime &dt)
{
    return dt.toUTC().toString("yyyy-MM-ddThh:mm:ss.zzzZ");
}

const QList<QueryEntry> GoogleDriveTools::getDbNameFilter(Database *db)
{
    QList<QueryEntry> filter;
    QueryEntry filterEntry(QueryEntry::QueryFilter::DBNAME, QStringList(
                               db->metadata()->name() + ".kdbx"));
    filter.append(filterEntry);
    return filter;
}

const QList<QueryEntry> GoogleDriveTools::getDbNameFilter(const QString &dbName)
{
    QList<QueryEntry> filter;
    QueryEntry filterEntry(QueryEntry::QueryFilter::DBNAME, QStringList(dbName));
    filter.append(filterEntry);
    return filter;
}

const bool GoogleDriveTools::hasLocalDatabase(const QString &dbName)
{
    return QFile(QDir::toNativeSeparators(config()->get("cloud/dbdir").toString()
                                          +"/" + dbName)).exists();
}

const QString GoogleDriveTools::getLocalDatabasePath(const QString &dbName)
{
    if (hasLocalDatabase(dbName))
        return QDir::toNativeSeparators(config()->get(
                                            "cloud/dbdir").toString() + "/" + dbName);
    else
        // TODO throw exception here
        return "";
}

const bool GoogleDriveTools::removeLocalDatabase(const QString &dbPath)
{
    bool result = true;
    if (QFile(dbPath).exists())
        QFile::remove(dbPath) ? result = true : result = false;
    return result;
}

void GoogleDriveTools::wait(const int waitTime)
{
    QEventLoop qE;
    QTimer tT;
    tT.setSingleShot(true);
    connect(&tT, SIGNAL(timeout()), &qE, SLOT(quit()));
    tT.start(waitTime);
    qE.exec();
}

// QDateTime RFC3339::fromRFC3339(const QString & dt) {
// return QDateTime::fromString(dt, "yyyy-MM-ddThh:mm:ss.zzzZ");
// }
