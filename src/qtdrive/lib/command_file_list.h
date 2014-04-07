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

#ifndef GOOGLE_DRIVE_COMMAD_FILE_LIST_H
#define GOOGLE_DRIVE_COMMAD_FILE_LIST_H

#include "authorized_command.h"
#include "file_info.h"

namespace GoogleDrive
{

class CommandFileListPrivate;

/**
 * @brief Lists the user's files
 */
class CommandFileList : public AuthorizedCommand
{
    Q_OBJECT
public:
    CommandFileList(Session* session);

    /**
     * If command success then this method returns received file list.
     * @return received file list.
     */
    FileInfoList files() const;

    QString fields() const;
    void setFields(const QString&);

public Q_SLOTS:
    /**
     * Executes command.
     * @sa about query https://developers.google.com/drive/search-parameters
     * @sa files
     */
    void exec(const QString& query = QString());

    /**
     * Get specified folder file list. The root folder id can be obtained by
     * CommandAbout.
     * @sa CommandAbout::rootFolder, files
     */
    void execForFolder(const QString& folderId);

protected:
    void reexecuteQuery();

private Q_SLOTS:
    void queryFinished();

private:
    Q_DECLARE_PRIVATE(CommandFileList)
};

}

#endif
