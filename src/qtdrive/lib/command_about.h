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

#ifndef GOOGLE_DRIVE_COMMAND_ABOUT_H
#define GOOGLE_DRIVE_COMMAND_ABOUT_H

#include "authorized_command.h"
#include "about_info.h"

namespace GoogleDrive
{

class CommandAboutPrivate;

/**
 * @brief Information about GD account.
 * @sa https://developers.google.com/drive/v2/reference/about/get
 */
class CommandAbout : public AuthorizedCommand
{
    Q_OBJECT
public:
    CommandAbout(Session* session);

    /**
     * Returns about information as a result of command execution.
     */
    const AboutInfo& resultInfo() const;

public Q_SLOTS:
    /**
     * Executing command.
     * @sa resultInfo
     */
    void exec();

protected:
    void reexecuteQuery();

private Q_SLOTS:
    void queryFinished();

private:
    Q_DECLARE_PRIVATE(CommandAbout)
};

}

#endif
