#ifndef GOOGLEDRIVESESSION_H
#define GOOGLEDRIVESESSION_H

#include <QtCore/QObject>
#include "../qtdrive/lib/session.h"
#include "core/Global.h"
#include "GDriveLoginPage.h"
#include "GDriveConstants.h"
using namespace GoogleDrive;
class GoogleDriveSession:public QObject
{
Q_OBJECT
public:
    GoogleDriveSession();
    ~GoogleDriveSession();
    static GoogleDriveSession* instance();
    static GoogleDriveSession* getEmptySession();
    Session* getNetworkSession();
public Q_SLOTS:
    void refreshTokenFinished();
private:
    void getRefreshToken();
    void waitForUserApproval();
    Session* session = Q_NULLPTR;
    static GoogleDriveSession* m_instance;
    Q_DISABLE_COPY(GoogleDriveSession)
    int refreshTokenErrorCode;
    QString refreshTokenErrorString;

Q_SIGNALS:
    void refreshSession(Session*);
    void pageApproved();

};

inline Session* getSession(){
    return GoogleDriveSession::instance()->getNetworkSession();
}

#endif // GOOGLEDRIVESESSION_H
