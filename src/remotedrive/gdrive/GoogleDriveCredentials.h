#ifndef GOOGLEDRIVECREDENTIALS_H
#define GOOGLEDRIVECREDENTIALS_H
#include "remotedrive/AuthCredentials.h"
#include "LoginPage.h"
#include "qtdrive/lib/session.h"

class GoogleDriveCredentials : public AuthCredentials
{
public:
    void init();
    void update();
    AuthCredentials* newInstance();
private:
    GoogleDriveCredentials();
    void raiseError(Errors::AuthorizationError errorType,const QString& description);

};

#endif // GOOGLEDRIVECREDENTIALS_H
