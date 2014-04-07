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

#ifndef GOOGLE_DRIVE_COMMAD_UPDATE_H
#define GOOGLE_DRIVE_COMMAD_UPDATE_H

#include "authorized_command.h"
#include "file_info.h"

namespace GoogleDrive
{

class CommandUpdatePrivate;

/**
 * @brief Updates file metadata.
 */
class CommandUpdate : public AuthorizedCommand
{
    Q_OBJECT
public:
    CommandUpdate(Session* session);

    /**
     * Result file information.
     * @return if command success then this method returns resulting file information.
     */
    FileInfo resultFileInfo() const;

    bool convert() const;
    void setConvert(bool);

public Q_SLOTS:
    /**
     * Executes command. A valid file ID is required.
     * @oaram info information that needs to be updated.
     */
    void exec(const FileInfo& info);

protected:
    void reexecuteQuery();

private Q_SLOTS:
    void queryFinished();

private:
    Q_DECLARE_PRIVATE(CommandUpdate)
};

}

#endif
