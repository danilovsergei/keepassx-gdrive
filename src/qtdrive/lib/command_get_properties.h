#ifndef COMMAND_GET_PROPERTIES_H
#define COMMAND_GET_PROPERTIES_H

#include "authorized_command.h"
#include "file_info.h"
#include <QtNetwork/QNetworkReply>

namespace GoogleDrive
{

class CommandGetFilePropertiesPrivate;

/**
 * @brief Get custom properties
 */
class CommandGetFileProperties : public AuthorizedCommand
{
    Q_OBJECT
public:
    CommandGetFileProperties(Session* session);
    QMap<QString,QString> getProperties();

public Q_SLOTS:

    void exec(const FileInfo& fileInfo);


protected:
    void reexecuteQuery();

private Q_SLOTS:
    void queryFinished();
    void queryFailed(QNetworkReply::NetworkError error);

private:
    Q_DECLARE_PRIVATE(CommandGetFileProperties)
};

}

#endif // COMMAND_GET_PROPERTIES_H
