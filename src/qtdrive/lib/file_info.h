/*******************************************************************************
 * libQtGoogleDrive - Google Drive API implementation based on the Qt Framework.
 * Copyright (C) 2012  Vladislav Navrocky
 *
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the BSD 2-Clause License.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifndef GOOGLE_DRIVE_FILE_INFO_H
#define GOOGLE_DRIVE_FILE_INFO_H

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QDateTime>
#include <QtCore/QList>

namespace GoogleDrive
{

/**
* @brief File information
*
* It holds full file information struct in QVariantMap format and provides
* convenient methods for read fields and build this struct from scratch.
*
* @sa CommandFileList, CommandGet
*/
class FileInfo
{
public:
    
    enum Labels {
        None        = 0,
        Hidden      = 1 << 0,
        Restricted  = 1 << 1,
        Starred     = 1 << 2,
        Trashed     = 1 << 3,
        Viewed      = 1 << 4,
    };
    
    typedef int LabelsMask;
    
    FileInfo();
    explicit FileInfo(const QVariantMap& data);
    FileInfo(const FileInfo& src);
    ~FileInfo();

    FileInfo& operator=(const FileInfo& src);

    bool isEmpty() const;

    const QVariantMap& rawData() const;
    QVariantMap& rawData();

    QString id() const;
    void setId(const QString&);

    QString title() const;
    void setTitle(const QString&);

    QString mimeType() const;
    QString description() const;
    bool isFolder() const;
    QDateTime createdDate() const;
    QDateTime modifiedDate() const;
    QDateTime modifiedByMeDate() const;
    QDateTime lastViewedByMeDate() const;
    bool isEditable() const;
    QUrl downloadUrl() const;
    QMap<QString, QUrl> exportList() const;
    
    QString extension(const QString& format = QString()) const;
	
    qint64 fileSize() const;

    QStringList parents() const;
    void setParents(const QStringList&);
    
    LabelsMask labels() const;
    bool hasLabel(Labels label) const;
    
private:
    struct Impl;
    Impl* d;
};

/**
* @typedef FileInfoList
* @brief List of file's informations.
*/
typedef QList<FileInfo> FileInfoList;

}

#endif
