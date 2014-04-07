#ifndef GOOGLEDRIVESESSION_H
#define GOOGLEDRIVESESSION_H

#include <QtCore/QObject>
#include "../qtdrive/lib/session.h"
#include "core/Global.h"
using namespace GoogleDrive;



class GoogleDriveSession:public QObject
{
Q_OBJECT
public:
    GoogleDriveSession();
    ~GoogleDriveSession();
    static GoogleDriveSession* instance();
    Session* getNetworkSession();
private:
    Session* session;
    static GoogleDriveSession* m_instance;

    Q_DISABLE_COPY(GoogleDriveSession)
};

inline Session* getSession(){
    return GoogleDriveSession::instance()->getNetworkSession();
}

#endif // GOOGLEDRIVESESSION_H
