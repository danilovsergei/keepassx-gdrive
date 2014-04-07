#include "GoogleDriveSession.h"
#include  <QtCore/QStringList>
#include <QtCore/QSettings>
#include "../qtdrive/test/options.h"
#include "QtNetwork/QNetworkAccessManager"
#include <QtCore/QDebug>
#include "../qtdrive/test/networkaccessmanagerproxy/networkaccessmanagerproxy.h"
#include "../qtdrive/test/networkaccessmanagerproxy/networkaccessviewer.h"
#include <QtCore/QDir>;

GoogleDriveSession* GoogleDriveSession::m_instance(Q_NULLPTR);
GoogleDriveSession::GoogleDriveSession()
{
    #ifdef Q_WS_WIN
    QString appDir = QDir::homePath() + "/Application Data/";
    #elif defined(Q_WS_X11)
    QString appDir = QDir::homePath() + "/.config/keepassx/";
    #endif

    QString homePath=QString(appDir+"qtgoogledrive-test.conf");
    qDebug() << QString("home path:"+homePath);
    QSettings s(homePath,QSettings::IniFormat);
    QString clientId = s.value(cClientId).toString();
    QString clientSecret = s.value(cClientSecret).toString();
    QString refreshToken = s.value(cRefreshToken).toString();

   QNetworkAccessManager* manager=new QNetworkAccessManager(this);
   //NetworkAccessManagerProxy* manager=new NetworkAccessManagerProxy();
   //NetworkAccessViewer* viewer=new NetworkAccessViewer();

   //manager->setNetworkAccessViewer(viewer);
   Session* session=new Session(manager,this);

   session->setClientId(clientId);
   session->setClientSecret(clientSecret);
   session->setRefreshToken(refreshToken);
   this->session=session;
}

GoogleDriveSession* GoogleDriveSession::instance(){
    if (!m_instance) {
        m_instance = new GoogleDriveSession();
    }
    return m_instance;
}

Session* GoogleDriveSession::getNetworkSession(){
    return session;
 }




GoogleDriveSession::~GoogleDriveSession(){

}
