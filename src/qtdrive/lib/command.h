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

#ifndef GOOGLE_DRIVE_COMMAND_H
#define GOOGLE_DRIVE_COMMAND_H

#include <QtCore/QObject>
#include <QtCore/QVariant>

class QNetworkRequest;
class QNetworkReply;

namespace GoogleDrive
{

class Session;
class CommandPrivate;

/**
 * @brief Base class for all GAPI commands.
 *
 */
class Command : public QObject
{
    Q_OBJECT
public:

    enum Error
    {
        /**
         * No error, command finished successfully
         */
        NoError,

        /**
         * Full cycle authorization is needed
         */
        AuthorizationNeeded,

        /**
         * Other unclassified errors
         */
        UnknownError
    };

    explicit Command(Session* session);
    ~Command();

    bool autoDelete() const;
    void setAutoDelete(bool v);

    Error error() const;
    QString errorString() const;

    /**
     * This method creates local event loop and waits for a command finish.
     * @param processUserInputEvents process user input events.
     * @returns false if command execution was failed.
     * @sa QEventLoop
     */
    bool waitForFinish(bool processUserInputEvents = true);

Q_SIGNALS:
    /**
     * Emitted when command is finished in any case (error or success).
     * Check error code provided by error().
     * @sa error, errorString
     */
    void finished();

    /**
     * Emitted if command execution is failed.
     * @param code error code
     * @sa errorString
     */
    void error(GoogleDrive::Command::Error code);

protected:
    Session* session() const;

    void emitStarted();
    void emitSuccess();
    void emitError(Error code, const QString& msg);
    void tryAutoDelete();

    bool checkJsonReplyError(const QVariantMap&);
    bool parseJsonReply(QNetworkReply* reply, QVariantMap&);
    bool checkInvalidReply(QNetworkReply* reply);

protected:
    CommandPrivate* const d_ptr;
    Command(CommandPrivate* dd, Session* session);
    Command(QObject* parent,CommandPrivate* d, Session *session);

private:
    Q_DECLARE_PRIVATE(Command)
};

}

#endif
