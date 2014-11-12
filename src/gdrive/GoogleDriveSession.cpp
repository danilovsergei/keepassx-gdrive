#include "GoogleDriveSession.h"
#include  <QtCore/QStringList>
#include <QtCore/QSettings>
#include "../qtdrive/test/options.h"
#include "QtNetwork/QNetworkAccessManager"
#include <QtCore/QDebug>
#include "../qtdrive/test/networkaccessmanagerproxy/networkaccessmanagerproxy.h"
#include "../qtdrive/test/networkaccessmanagerproxy/networkaccessviewer.h"
#include <QtCore/QDir>;
#include "core/Config.h"

GoogleDriveSession *GoogleDriveSession::m_instance(Q_NULLPTR);
const QString CLIENT_ID     = "521376217688.apps.googleusercontent.com";
const QString CLIENT_SECRET = "SXF0r3tMchlKw1WXD6rzZldJ";

GoogleDriveSession::GoogleDriveSession()
{
  QNetworkAccessManager *manager = new QNetworkAccessManager(this);

  session = new Session(manager, this);
  session->setClientId(CLIENT_ID);
  session->setClientSecret(CLIENT_SECRET);
}

void GoogleDriveSession::getRefreshToken() {
  QString refreshToken =  config()->get("cloud/refresh_token").toString();

  if (refreshToken.isEmpty()) {
    // signal emitted to gui thread
    Q_EMIT(refreshSession(session));

    // wait until user will approve actions through the UI dialog
    waitForUserApproval();

    if (refreshTokenErrorCode != Errors::NO_ERROR) {
      qDebug() <<
      QString("Failed to get authorization token. Error code %1 : %2").arg(QString::number(
                                                                             refreshTokenErrorCode),
                                                                           refreshTokenErrorString);
    }
    else {
      qDebug() << "Successfully got authorization token";
      qDebug() << "Update user config";
      config()->set("cloud/refresh_token",session->refreshToken());
    }

    // TODO insert proper error handling through showing error dialog to user
    // also it's a good idea to show system notification
  } else {
    session->setRefreshToken(refreshToken);
  }
}

void GoogleDriveSession::waitForUserApproval() {
  QEventLoop qE;
  QTimer     tT;

  tT.setSingleShot(true);
  connect(&tT,  SIGNAL(timeout()),      &qE, SLOT(quit()));
  connect(this, SIGNAL(pageApproved()), &qE, SLOT(quit()));
  tT.start(LOGIN_WAIT_TIMEOUT);
  qE.exec();
}

void GoogleDriveSession::refreshTokenFinished() {
  GDriveLoginPage *loginPage = qobject_cast<GDriveLoginPage *>(sender());
  refreshTokenErrorCode   = static_cast<int>(loginPage->error());
  refreshTokenErrorString = loginPage->errorString();
  Q_EMIT pageApproved();
}

GoogleDriveSession * GoogleDriveSession::getEmptySession() {
  if (!m_instance) {
    m_instance = new GoogleDriveSession();
  }
  return m_instance;
}

GoogleDriveSession * GoogleDriveSession::instance() {
  if (!m_instance) {
    m_instance = GoogleDriveSession::getEmptySession();
  }

  if (m_instance->session->accessToken().isEmpty()) { // TODO check expiration time here as well
    m_instance->getRefreshToken();
  }
  return m_instance;
}

Session * GoogleDriveSession::getNetworkSession() {
  return session;
}

GoogleDriveSession::~GoogleDriveSession() {}
