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

#ifndef GOOGLE_DRIVE_SESSION_H
#define GOOGLE_DRIVE_SESSION_H

#include <QtCore/QObject>
#include <QtCore/QDebug>

class QNetworkAccessManager;

namespace GoogleDrive
{

class Command;
class CommandOAuth2;
class CommandRefreshToken;
class SessionPrivate;

/**
 * @brief GoogleDrive session
 *
 * Provides environment for executing GD commands. It holds pointer to
 * QNetworkAccessManager and access tokens.
 */
class Session : public QObject
{
    Q_OBJECT
public:
    /**
     * Session constructor.
     * @param manager QNetworkAccessManager used to executing network queries.
     *        Session don't own provided manager.
     * @param parent
     */
    Session(QObject* parent = 0);
    ~Session();

    /**
     * Returns assigned QNetworkAccessManager
     */
    QNetworkAccessManager* networkManager() const;

    QString clientId() const;
    void setClientId(const QString& v);

    QString clientSecret() const;
    void setClientSecret(const QString& v);

    QString refreshToken() const;
    void setRefreshToken(const QString& v);

    QString accessToken() const;

    /**
     * Returns number of seconds after access_token will be expired.
     * This value is valid after first time authorization or after executing any command.
     */
    int accessTokenExpiresIn() const;

Q_SIGNALS:
    /**
     * This signal is emitted when command started.
     * @param command started to execution command.
     */
    void started(GoogleDrive::Command* command);

    /**
     * Signal is emitted when command is finished successfully or failed.
     * @param successCommand a command that execution is finished.
     * @sa Command::error, Command::errorString
     */
    void finished(GoogleDrive::Command* successCommand);

    /**
     * Signal is emitted when command execution is failed.
     * @param successCommand a command that execution is failed.
     * @sa Command::error, Command::errorString
     */
    void error(GoogleDrive::Command* failedCommand);

private:
    friend class Command;
    friend class CommandOAuth2;
    friend class CommandRefreshToken;

    void setAccessToken(const QString& v);
    void setAccessTokenExpiresIn(int);

    SessionPrivate* d_ptr;
    Q_DECLARE_PRIVATE(Session)
};

}

#endif
