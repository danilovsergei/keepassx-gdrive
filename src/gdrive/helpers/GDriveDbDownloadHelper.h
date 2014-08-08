#ifndef GDRIVEDBDOWNLOADHELPER_H
#define GDRIVEDBDOWNLOADHELPER_H
#include "../../qtdrive/lib/file_info.h"
#include "../GoogleDriveApi.h"
#include <QtCore/QList>
#include "../QueryEntry.h"
#include <QtCore/QDebug>

class GDriveDbDownloadHelper
{
public:
    GDriveDbDownloadHelper();
    ~GDriveDbDownloadHelper();
    static const GoogleDrive::FileInfo& downloadDatabase(const QString& dbName, const QString& lastModified);
};

#endif // GDRIVEDBDOWNLOADHELPER_H
