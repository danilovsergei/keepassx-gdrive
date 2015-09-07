#ifndef LOGINPAGE_H
#define LOGINPAGE_H
#include <QObject>
#include <QPair>
#include <QWebView>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>
#include <qtdrive/lib/command_oauth2.h>
#include "qtdrive/lib/session.h"
#include <remotedrive/Errors.h>
#include <QApplication>
#include "GDriveConstants.h"
using namespace GoogleDrive;
class LoginPage : public QObject
{
    Q_OBJECT
private:
    const QString approvalPageName = "https://accounts.google.com/o/oauth2/approval";
    const QPair<int, int> geometry = QPair<int, int>(800, 800);

    QWebView *approvalView;
    GoogleDrive::Session *session;
    QString errorStringValue;
    Errors::AuthorizationError errorValue;
    QString authorizationCode;
    bool approved = false;
public:
    LoginPage();
    QString errorString();
    Errors::AuthorizationError error();
public Q_SLOTS:
    void updateCredentials(Session &session);
private Q_SLOTS:
    void loadFinished(const bool &finished);
    void waitForUserApproval();
    void raiseError(Errors::AuthorizationError errorType, const QString &description);
Q_SIGNALS:
    void pageApproved();
    void finished();
};

#endif // LoginPage_H
