#include "GDriveDbDownloadHelper.h"

GDriveDbDownloadHelper::GDriveDbDownloadHelper()
{
}

GDriveDbDownloadHelper::~GDriveDbDownloadHelper() {

}


const GoogleDrive::FileInfo& GDriveDbDownloadHelper::downloadDatabase(const QString& dbName,const QString& lastModified) {
    QList<QueryEntry> filter;
    QueryEntry entry(QueryEntry::QueryFilter::DBNAME,QStringList(dbName));
    QueryEntry entry1(QueryEntry::QueryFilter::LAST_MODIFIED,QStringList(lastModified));
    filter.append(entry);
    filter.append(entry1);


    QString result=QueryEntry::getEntries(filter);
    googleDriveApi()->getDatabasesPar(filter);

}
