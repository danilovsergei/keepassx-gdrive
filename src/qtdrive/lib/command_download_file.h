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

#ifndef GOOGLE_DRIVE_COMMAND_DOWNLOAD_FILE_H
#define GOOGLE_DRIVE_COMMAND_DOWNLOAD_FILE_H

#include "authorized_command.h"

class QIODevice;
class QUrl;

namespace GoogleDrive
{

class CommandDownloadFilePrivate;

// TODO: Download realization with Partial GET
/**
 * @brief Download file content
 */
class CommandDownloadFile : public AuthorizedCommand
{
    Q_OBJECT
public:
    CommandDownloadFile(QObject *parent, Session* session);
    CommandDownloadFile(Session* session);
    ~CommandDownloadFile();

    /**
     * This method returns current receive buffer size. By default receive buffer
     * size is 16 kB.
     * @sa setBufferSize
     */
    qint64 bufferSize() const;

    /**
     * This method change receive buffer size. Default buffer size is 16 kB
     * @param size new receive buffer size
     * @sa bufferSize
     */
    void setBufferSize(qint64 size);

public Q_SLOTS:
    /**
     * Start downloading process.
     * @param downloadUrl download or export url provided by FileInfo::downloadUrl
     * @param out output device for storing received file content. It can be a
     *        QFile, QBuffer, etc.
     */
    void exec(const QUrl& downloadUrl, QIODevice* out);

Q_SIGNALS:
    /**
     * This signal is emitted during downloading process.
     * @param bytesReceived current amount of received data
     * @param total total amount of data to be received
     */
    void progress(qint64 bytesReceived, qint64 total);

protected:
    void reexecuteQuery();

private Q_SLOTS:
    void requestFinished();
    void readyRead();

private:
    Q_DECLARE_PRIVATE(CommandDownloadFile)
};

}

#endif
