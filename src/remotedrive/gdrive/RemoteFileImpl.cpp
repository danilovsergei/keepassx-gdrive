#include "RemoteFileImpl.h"

RemoteFileImpl::RemoteFileImpl()
{
}

RemoteFile RemoteFileImpl::fromGDriveFileInfo(const FileInfo &fi)
{
    RemoteFile remoteFile;

    remoteFile.setTitle(fi.title());
    remoteFile.setCreatedDate(fi.createdDate());
    remoteFile.setFileSize(fi.fileSize());
    remoteFile.setModifiedDate(fi.modifiedDate());
    remoteFile.setParents(fi.parents());
    return remoteFile;
}

RemoteFileList RemoteFileImpl::fromGDriveFileInfoList(const FileInfoList &files)
{
    RemoteFileList fileList;

    Q_FOREACH(const FileInfo &file, files) {
        RemoteFile remoteFile = RemoteFileImpl::fromGDriveFileInfo(file);

        fileList.append(remoteFile);
    }
    return fileList;
}

FileInfo RemoteFileImpl::toGDriveFileInfo(const RemoteFile &remoteFile)
{
    QVariantMap data;
    const char *cId = "id";
    const char *cTitle = "title";
    const char *cParents = "parents";

    data.insert(cTitle, remoteFile.getTitle());
    data.insert(cParents, remoteFile.getParents());
    data.insert(cId, remoteFile.getId());
    FileInfo fi(data);
    return fi;
}

FileInfoList RemoteFileImpl::toGDriveFileInfoList(const RemoteFileList &remoteFileList)
{
    FileInfoList fileList;

    Q_FOREACH(const RemoteFile &file, remoteFileList) {
        FileInfo fi = RemoteFileImpl::toGDriveFileInfo(file);
        fileList.append(fi);
    }
    return fileList;
}
