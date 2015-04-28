#ifndef REMOTEFILEIMPL_H
#define REMOTEFILEIMPL_H
#include "remotedrive/RemoteFile.h"
#include <QtCore/QObject>
#include "qtdrive/lib/file_info.h"
using namespace GoogleDrive;
class RemoteFileImpl : public QObject
{
    Q_OBJECT
public:
    RemoteFileImpl();
    static RemoteFile fromGDriveFileInfo(const FileInfo &fi);
    static RemoteFileList fromGDriveFileInfoList(const FileInfoList &files);
    static FileInfo toGDriveFileInfo(const RemoteFile &remoteFile);
    static FileInfoList toGDriveFileInfoList(const RemoteFileList &remoteFileList);
};

#endif // REMOTEFILEIMPL_H
