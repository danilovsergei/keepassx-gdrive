#include "GoogleDriveTools.h"



QString GoogleDriveTools::toRFC3339(const QDateTime& dt) {
    return dt.toUTC().toString("yyyy-MM-ddThh:mm:ss.zzzZ");
}


const QList<QueryEntry> GoogleDriveTools::getDbNameFilter(Database* db) {
    QList<QueryEntry> filter;
    QueryEntry filterEntry(QueryEntry::QueryFilter::DBNAME,QStringList(db->metadata()->name()+".kdbx"));
    filter.append(filterEntry);
    return filter;
}

const QList<QueryEntry> GoogleDriveTools::getDbNameFilter(const QString& dbName) {
    QList<QueryEntry> filter;
    QueryEntry filterEntry(QueryEntry::QueryFilter::DBNAME,QStringList(dbName));
    filter.append(filterEntry);
    return filter;
}

//QDateTime RFC3339::fromRFC3339(const QString & dt) {
//    return QDateTime::fromString(dt, "yyyy-MM-ddThh:mm:ss.zzzZ");
//}
