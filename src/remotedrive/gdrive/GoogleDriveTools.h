#ifndef GOOGLEDRIVETOOLS_H
#define GOOGLEDRIVETOOLS_H
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <remotedrive/gdrive/QueryEntry.h>
#include <core/Database.h>
#include <core/Metadata.h>
#include "core/Config.h"
#include <QtCore/QDir>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtCore/QDebug>

class GoogleDriveTools : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief toRFC3339 - Converts Qt QDateTime to used by Google drive RFC3339 format which excludes any ambigios cases
     * @param dt - time in Qt QDateTime format
     * @return - QString time in RFC3339 format. Note function converts time to UTC to simplify process of parsing timezones during convert
     */
    static QString toRFC3339(const QDateTime &dt);

    /**
     * @brief getDbNameFilter - creates filter for google drive api using only database name
     * @param db - pointer to the source database name will be taken from
     * @return  - filter object with dbname filter entry and parameter
     */
    static const QList<QueryEntry> getDbNameFilter(Database *db);
    static const QList<QueryEntry> getDbNameFilter(const QString &dbName);
    /**
     * @brief hasLocalDatabase checks whether database with dbName exists in configured local folder
     * @param dbName database name to check
     */
    static const bool hasLocalDatabase(const QString &dbName);
    /**
     * @brief getLocalDatabasePath gets the full path to local database
     * @param dbName name of the database in format testdb.kdbx
     * @return full path to local database
     */
    static const QString getLocalDatabasePath(const QString &dbName);

    /**
     * @brief removeLocalDatabase removes local database file if it's exists
     * @param dbPath full path to local file
     * @return false if failed to remove.othewise true
     */
    static const bool removeLocalDatabase(const QString &dbPath);
    /**
     * @brief GoogleDriveTools::pauses current event loop for specified amount of time.
     * @param waitTime - milliseconds to wait
     */
    void wait(const int waitTime);
};

#endif // GOOGLEDRIVETOOLS_H
