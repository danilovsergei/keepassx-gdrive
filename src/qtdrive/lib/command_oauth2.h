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

#ifndef GOOGLE_DRIVE_COMMAND_OAUTH2_H
#define GOOGLE_DRIVE_COMMAND_OAUTH2_H

#include <QString>
#include <QUrl>

#include "command.h"

namespace GoogleDrive
{

class CommandOAuth2Private;

/**
 * @brief First-time OAuth2 authorization for installed applications
 *
 * Authenticating users with OAuth 2.0. At the moment only supported scenario
 * for installed applications.
 *
 * @sa https://developers.google.com/drive/about-auth
 * @sa https://developers.google.com/accounts/docs/OAuth2InstalledApp
 */
class CommandOAuth2 : public Command
{
    Q_OBJECT
public:
    /**
     * Access scope.
     * @sa https://developers.google.com/drive/scopes
     */
    enum AccessScope
    {
        /**
         * Per-file access to files created or opened by the app.
         */
        FileScope = 1,

        /**
         * Allows apps read-only access to the list of Drive apps a user
         * has installed.
         */
        AppsReadonyScope = 1 << 1,

        /**
         * Allows read-only access to file metadata and file content
         */
        ReadonlyScope = 1 << 2,

        /**
         * Allows read-only access to file metadata, but does not allow any
         * access to read or download file content
         */
        ReadonlyMetadataScope = 1 << 3,

        /**
         * Full access to user drive files.
         */
        FullAccessScope = 1 << 4
    };
    Q_DECLARE_FLAGS(AccessScopes, AccessScope)

    CommandOAuth2(Session*);

    QString scope() const;
    void setScope(const QString& v);
    void setScope(AccessScopes);

    /**
     * Returns OAuth2 dialog url. You have to open this url in the web browser
     * for further authorization steps.
     * @sa QDesktopServices::openUrl
     */
    QUrl getLoginUrl() const;

public Q_SLOTS:
    /**
     * Requesting refresh and access tokens using authorization code retrieved
     * from browser.
     * @sa finished, Command::error()
     */
    void requestAccessToken(const QString& authCode);

private Q_SLOTS:
    void requestAccessTokenFinished();

private:
    Q_DECLARE_PRIVATE(CommandOAuth2)
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(GoogleDrive::CommandOAuth2::AccessScopes)

#endif
