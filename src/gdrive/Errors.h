#ifndef ERRORS_H
#define ERRORS_H
#include <QtCore/QPair>
#include <typeinfo>
#include <QtCore/QDebug>
class Errors
{
public:
    enum SyncError {AMBIGIOUS_DB=1,NETWORK_PROBLEM=2,SYNC_PROBLEM=3,KEY_PROBLEM=4};
    enum DownloadError {COMMON_DOWNLOAD_PROBLEM=21};
    enum FileError {OPEN_FILE_PROBLEM=41};
    Errors();
};

#endif // ERRORS_H
