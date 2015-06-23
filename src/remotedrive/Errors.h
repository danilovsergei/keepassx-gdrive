#ifndef ERRORS_H
#define ERRORS_H
class Errors
{
public:
    enum {
        NO_ERROR = 0
    };
    enum SyncError {
        AMBIGIOUS_DB = 1, NETWORK_PROBLEM = 2, SYNC_PROBLEM = 3, KEY_PROBLEM = 4
    };
    enum DownloadError {
        COMMON_DOWNLOAD_PROBLEM = 21, DBNAME_ERROR_PROBLEM = 22, DOWNLOAD_COMMAND_PROBLEM = 23
    };
    enum FileError {
        OPEN_FILE_PROBLEM = 41, LIST_FILES_PROBLEM = 42, UPLOAD_FILES_PROBLEM = 43, SET_CUSTOM_PROPERTY_PROBLEM = 44, DELETE_DB_PROBLEM = 45
    };
    enum AuthorizationError {
        GENERAL_AUTH_PROBLEM = 60, USER_APPROVE_PROBLEM = 61
    };
    enum RemoteApiError {
        API_INITIALIZATION_PROBLEM = 80
    };
    enum InternalError {
        EMPTY_ERROR_CODE = 100
    };
    Errors();
};

#endif // ERRORS_H
