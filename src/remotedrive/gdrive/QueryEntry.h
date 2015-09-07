#ifndef QUERYENTRY_H
#define QUERYENTRY_H
#include <QStringList>
#include <QString>
#include <QMap>
#include "remotedrive/gdrive/GDriveConstants.h"
class QueryEntry
{
public:
    enum class QueryFilter
    {
        DBNAME, LAST_MODIFIED
    };
    static bool filtersInitialized;
    static QMap<QueryEntry::QueryFilter, QString> filters;
    QueryEntry(const QueryFilter &entryType, const QStringList &parameters);
    static const QString getEntries(const QList<QueryEntry> &entries);
    static const QMap<QueryFilter, QString> &getFilters();
private:
    QStringList parameters;
    QueryFilter type;
};

#endif // QUERYENTRY_H
