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

#ifndef GOOGLE_DRIVE_ABOUT_INFO_H
#define GOOGLE_DRIVE_ABOUT_INFO_H

#include <QVariant>

namespace GoogleDrive
{

class AboutInfoPrivate;

/**
 * @brief An item with user information and settings.
 *
 * @sa https://developers.google.com/drive/v2/reference/about#resource
 */
class AboutInfo
{
public:
    AboutInfo();
    explicit AboutInfo(const QVariantMap& data);
    AboutInfo(const AboutInfo& src);
    ~AboutInfo();
    AboutInfo& operator=(const AboutInfo&);

    /**
     * Access to the raw data of about item.
     */
    const QVariantMap& rawData() const;

    /**
     * The name of the current user.
     */
    QString name() const;

    /**
     * The id of the root folder.
     */
    QString rootFolderId() const;

    /**
     * The total number of quota bytes.
     */
    quint64 quotaBytesTotal() const;

    /**
     * The number of quota bytes used.
     */
    quint64 quotaBytesUsed() const;

    /**
     * The number of quota bytes used by trashed items.
     */
    quint64 quotaBytesUsedInTrash() const;

    /**
     * The largest change id.
     */
    int largestChangeId() const;

    /**
     * The domain sharing policy for the current user.
     */
    QString domainSharingPolicy() const;

    /**
     * The current user's ID as visible in the permissions collection.
     */
    QString permissionId() const;

    /**
     * A boolean indicating whether the authenticated app is installed by the authenticated user.
     */
    bool isCurrentAppInstalled() const;

private:
    AboutInfoPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(AboutInfo)
};

}

#endif
