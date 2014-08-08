#ifndef GDRIVEGETDBHELPER_H
#define GDRIVEGETDBHELPER_H
#include "../QueryEntry.h"
#include "../GoogleDriveApi.h"
#include "../../qtdrive/lib/command_file_list.h"
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtCore/QDebug>
class GDriveGetDbHelper: public QObject
{
    Q_OBJECT
public:
    GDriveGetDbHelper(const QList<QueryEntry> &queryFilter);
    const FileInfoList getDatabases();

private Q_SLOTS:
    void DbListLoaded(const GoogleDrive::FileInfoList dbList);

private:
    void waitForFinish();
    GoogleDrive::FileInfoList dbList;
    bool loaded=false;

};

#endif // GDRIVEGETDBHELPER_H
