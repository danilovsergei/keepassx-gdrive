#ifndef BASECOMMAND_H
#define BASECOMMAND_H

#include <remotedrive/RemoteCommand.h>
#include <qtdrive/lib/session.h>
#include <QThreadStorage>
#include <QNetworkAccessManager>
#include <remotedrive/gdrive/GDriveConstants.h>
#include <QTimer>
#include <remotedrive/AuthCredentials.h>

using namespace GoogleDrive;
class BaseCommand : public KeePassxDriveSync::RemoteCommand
{
  Q_OBJECT
public:
  BaseCommand(AuthCredentials *creds);
protected:
  Session *getSession();
  AuthCredentials *creds = Q_NULLPTR;
private:
  static QThreadStorage<Session *> session;
  void waitForCredsRefresh();

Q_SIGNALS:
  void updateCredentials();
};

#endif // BASECOMMAND_H
