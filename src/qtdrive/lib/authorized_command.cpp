#include "authorized_command.h"

#include <QNetworkReply>
#include <QPointer>

#include "command_refresh_token.h"
#include "session.h"

namespace GoogleDrive
{
typedef QPointer<GoogleDrive::CommandRefreshToken> CommandRefreshToken_p;
}

Q_DECLARE_METATYPE(GoogleDrive::CommandRefreshToken_p)

namespace GoogleDrive
{

namespace
{
const char* cRefreshCommand = "RefreshCommand";
}

AuthorizedCommand::AuthorizedCommand(Session* session)
    : Command(session)
{
}

bool AuthorizedCommand::checkForRefreshToken(const QVariantMap& map)
{
    const QString cError("error");
    if (!map.contains(cError))
        return false;

    const QVariantMap& errorMap = map.value(cError).toMap();
    const QString& code = errorMap.value("code").toString();
    const QString& message = errorMap.value("message").toString();

    if (code == "403")
    {
        // full reauthorization is needed
        emitError(AuthorizationNeeded, message);
        return true;
    }

    if (code != "401")
        return false;

    refreshToken();
    return true;
}

bool AuthorizedCommand::checkInvalidReplyAndRefreshToken(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
        return false;

    QVariantMap map;
    if (parseJsonReply(reply, map))
    {
        if (checkForRefreshToken(map))
            return true;

        if (checkJsonReplyError(map))
            return true;
    }
    emitError(UnknownError, reply->errorString());
    return true;
}

void AuthorizedCommand::refreshToken()
{
    CommandRefreshToken_p cmd = session()->property(cRefreshCommand).value<CommandRefreshToken_p>();
    if (!cmd)
    {
        cmd = new CommandRefreshToken(session());
        cmd->setAutoDelete(true);
        cmd->exec();
        session()->setProperty(cRefreshCommand, QVariant::fromValue(CommandRefreshToken_p(cmd)));
    }
    connect(cmd, SIGNAL(finished()), SLOT(refreshTokenFinished()));
}

AuthorizedCommand::AuthorizedCommand(CommandPrivate* d, Session* s)
    : Command(d, s)
{
}

void AuthorizedCommand::refreshTokenFinished()
{
    CommandRefreshToken* cmd = qobject_cast<CommandRefreshToken*>(sender());
    if (cmd->error() == NoError)
        reexecuteQuery();
    else
        emitError(AuthorizationNeeded, cmd->errorString());
}

void AuthorizedCommand::executeQuery()
{
    emitStarted();
    if (session()->accessToken().isEmpty())
    {
        if (session()->refreshToken().isEmpty())
            emitError(AuthorizationNeeded, tr("Refresh token is empty, "
                                              "you should to process authorization."));
        else
            refreshToken();
    }
    else
        reexecuteQuery();
}

}
