#include "command_about.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "command_private.h"
#include "about_info.h"
#include "tools.h"
#include "session.h"

using namespace GoogleDrive::Internal;

namespace GoogleDrive
{

class CommandAboutPrivate : public CommandPrivate
{
public:
    AboutInfo resultInfo;
};

CommandAbout::CommandAbout(Session *session)
    : AuthorizedCommand(new CommandAboutPrivate, session)
{
}

const AboutInfo &CommandAbout::resultInfo() const
{
    Q_D(const CommandAbout);
    return d->resultInfo;
}

void CommandAbout::exec()
{
    executeQuery();
}

void CommandAbout::reexecuteQuery()
{
    Q_D(CommandAbout);
    QString url("https://www.googleapis.com/drive/v2/about");

    QNetworkRequest request(url);
    setRequestAccessToken(request, session()->accessToken());

    QNetworkReply* reply = session()->networkManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(queryFinished()));
    reply->setParent(this);
}

void CommandAbout::queryFinished()
{
    Q_D(CommandAbout);
    tryAutoDelete();
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;
    reply->deleteLater();

    if (checkInvalidReplyAndRefreshToken(reply))
        return;

    QVariantMap map;
    if (!parseJsonReply(reply, map))
        return;

    d->resultInfo = AboutInfo(map);

    emitSuccess();
}

}
