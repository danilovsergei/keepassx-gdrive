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

#ifndef GOOGLE_DRIVE_COMMAND_UPLOAD_FILE_H
#define GOOGLE_DRIVE_COMMAND_UPLOAD_FILE_H

#include "authorized_command.h"
#include "file_info.h"

namespace GoogleDrive
{

class CommandUploadFilePrivate;

/**
 * @brief Uploads file contents
 *
 * This command uploads file contents with metadata to the GD.
 * If provided FileInfo contains valid file ID than file metadata and data will
 * be rewritten, otherwise the new file will be created.
 *
 * After uploading
 */
class CommandUploadFile : public AuthorizedCommand
{
    Q_OBJECT
public:
    enum UploadType
    {
        /**
         * Uploading with single request. Acceptable if the data you are sending
         * is small enough to upload again in its entirety if the connection
         * fails.
         */
        Multipart,

        /**
         * This method allows you to resume an upload operation after a
         * communication failure has interrupted the flow of data.
         * @warning Not realized at this moment
         */
        Resumable
    };

    CommandUploadFile(Session* session);

    /**
     * Returns uploading type. Default value is Multipart.
     * @sa setUploadType
     */
    UploadType uploadType() const;

    /**
     * Change uploading type.
     * @sa uploadType
     */
    void setUploadType(UploadType);

    /**
     * Resulting file info after successfully executed command.
     * @return FileInfo information about uploaded file.
     */
    const FileInfo& resultFileInfo() const;

public Q_SLOTS:
    /**
     * Execute upload command. After successfully execution resultFileInfo will
     * contain updated file information.
     * @param fileInfo file meta information. If FileInfo::id is not empty then
     *        command will overwrite file with specified id, otherwise the new
     *        file will be created.
     * @param fileData QIODevice that provides file data. Command doesn't own
     *        this object, so it must exists during command execution.
     * @sa finished, error, progress
     */
    void exec(const FileInfo& fileInfo, QIODevice* fileData);

Q_SIGNALS:
    /**
     * This signal is emitted during uploading process.
     * @param bytesUploaded current amount of uploaded data
     * @param total total amount of data to be upload
     */
    void progress(qint64 bytesUploaded, qint64 total);

protected:
    void reexecuteQuery();

private Q_SLOTS:
    void requestFinished();

private:
    void multipartUpload();

    Q_DECLARE_PRIVATE(CommandUploadFile)
};

}

#endif
