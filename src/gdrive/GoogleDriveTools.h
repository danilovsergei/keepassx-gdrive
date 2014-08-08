#ifndef GOOGLEDRIVETOOLS_H
#define GOOGLEDRIVETOOLS_H
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <gdrive/QueryEntry.h>
#include <core/Database.h>
#include <core/Metadata.h>

class GoogleDriveTools
{
public:
    /**
     * @brief toRFC3339 - Converts Qt QDateTime to used by Google drive RFC3339 format which excludes any ambigios cases
     * @param dt - time in Qt QDateTime format
     * @return - QString time in RFC3339 format. Note function converts time to UTC to simplify process of parsing timezones during convert
     */
    static QString toRFC3339(const QDateTime& dt);

    /**
     * @brief getDbNameFilter - creates filter for google drive api using only database name
     * @param db - pointer to the source database name will be taken from
     * @return  - filter object with dbname filter entry and parameter
     */
    static const QList<QueryEntry> getDbNameFilter(Database* db);
    static const QList<QueryEntry> getDbNameFilter(const QString& dbName);
};

#endif // GOOGLEDRIVETOOLS_H
