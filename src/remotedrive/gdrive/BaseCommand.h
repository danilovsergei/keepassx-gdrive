#ifndef BASECOMMAND_H
#define BASECOMMAND_H

#include <remotedrive/Command.h>
#include <qtdrive/lib/session.h>
#include <QtCore/QThreadStorage>
#include <QtNetwork/QNetworkAccessManager>
#include <remotedrive/gdrive/GDriveConstants.h>
#include <QtCore/QTimer>
#include <remotedrive/AuthCredentials.h>

using namespace GoogleDrive;
class BaseCommand : public KeePassxDriveSync::Command
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
