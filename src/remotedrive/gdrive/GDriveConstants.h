#ifndef GDRIVECONSTANTS_H
#define GDRIVECONSTANTS_H
#include <QtCore/QString>
static const QString CLIENT_ID_VALUE     = "521376217688.apps.googleusercontent.com";
static const QString CLIENT_SECRET_VALUE = "SXF0r3tMchlKw1WXD6rzZldJ";

static const QString LAST_MODIFIED="LAST_MODIFIED";
static const int LOGIN_WAIT_TIMEOUT = 2*60*1000;

static const QString OPTION_DB_NAME = "DB_NAME";
static const QString OPTION_DB_DIR = "DB_DIR";
// full absolute db name including path
static const QString OPTION_ABSOLUTE_DB_NAME  = "ABSOLUTE_DB_NAME";
// FileInfo GDrive specific object with information about cloud file
static const QString OPTION_FI = "FI";
// API independent RemoteFile object
static const QString OPTION_REMOTE_FILE = "REMOTE_FILE";
// pointer to Database* object
static const QString OPTION_DB_POINTER = "DB_POINTER";
static const QString OPTION_LAST_MODIFIED_TIME = "LAST_MODIFIED_TIME";
static const QString OPTION_DB_FILTER = "DB_FILTER";
// parent name in the GoogleDrive
static const QString OPTION_PARENT_NAME  = "PARENT_NAME";

static const QString CLIENT_ID = "CLIENT_ID";
static const QString CLIENT_SECRET = "CLIENT_SECRET";
static const QString REFRESH_TOKEN = "REFRESH_TOKEN";


#endif // GDRIVECONSTANTS_H
