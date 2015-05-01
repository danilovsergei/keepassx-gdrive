#include "UploadCommand.h"

UploadCommand::UploadCommand(Session *session)
{

}

void UploadCommand::execute(const QVariantMap& options) {
    Database *localDb = parseOption<Database *>(options, OPTION_DB_POINTER);
    const QString localDbPath = parseOption<QString>(options, OPTION_ABSOLUTE_DB_NAME);
    const QDateTime lastModified = parseOption<QDateTime>(options, OPTION_LAST_MODIFIED_TIME);
    const QString parent = parseOption<QString>(option, OPTION_PARENT_NAME);

    UploadCommand uploadCommand(session);

}


UploadCommand::~UploadCommand()
{

}

