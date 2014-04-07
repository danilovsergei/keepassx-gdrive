#ifndef COMMAND_SET_PROPERTIES_H
#define COMMAND_SET_PROPERTIES_H

#include "authorized_command.h"
#include "file_info.h"
#include <QtNetwork/QNetworkReply>

namespace GoogleDrive
{

class CommandSetFilePropertiesPrivate;

/**
 * @brief Set custom properties
 */
class CommandSetFileProperties : public AuthorizedCommand
{
    Q_OBJECT
public:
    CommandSetFileProperties(Session* session);

public Q_SLOTS:

    void exec(const FileInfo& fileInfo,const QMap<QString,QString>& fields);
    void exec(const FileInfo& fileInfo,const QPair<QString,QString>& field);


protected:
    void reexecuteQuery();

private Q_SLOTS:
    void queryFinished();
    void queryFailed(QNetworkReply::NetworkError error);

private:
    Q_DECLARE_PRIVATE(CommandSetFileProperties);
    QString currentKey;
};

}

#endif // COMMAND_SET_PROPERTIES_H
