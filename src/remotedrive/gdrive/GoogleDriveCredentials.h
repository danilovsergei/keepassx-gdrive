#ifndef GOOGLEDRIVECREDENTIALS_H
#define GOOGLEDRIVECREDENTIALS_H
#include "remotedrive/AuthCredentials.h"
#include "LoginPage.h"
#include "qtdrive/lib/session.h"
#include "core/Config.h"
#include <QScopedPointer>
#include "remotedrive/gdrive/GDriveConstants.h"

class GoogleDriveCredentials : public AuthCredentials
{
public:
    void init();
    void update();
    GoogleDriveCredentials(QObject *parent);
private:

    void raiseError(Errors::AuthorizationError errorType, const QString &description);
};

#endif // GOOGLEDRIVECREDENTIALS_H
