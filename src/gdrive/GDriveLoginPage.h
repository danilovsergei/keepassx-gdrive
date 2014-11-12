#ifndef GDRIVELOGINPAGE_H
#define GDRIVELOGINPAGE_H
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtWebKit/QWebView>
#include <QtCore/QDebug>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <qtdrive/lib/command_oauth2.h>
#include "../qtdrive/lib/session.h"
#include <gdrive/Errors.h>
#include <QtGui/QApplication>
#include "GDriveConstants.h"
using namespace GoogleDrive;
class GDriveLoginPage : public QObject
{
Q_OBJECT
private:
    const QString approvalPageName = "https://accounts.google.com/o/oauth2/approval";
    const QPair<int,int> geometry = QPair<int,int>(800,800);

    QWebView* approvalView;
    GoogleDrive::Session* session;
    QString errorStringValue;
    Errors::AuthorizationError errorValue;
    QString authorizationCode;
    bool approved = false;
public:
    GDriveLoginPage();
    QString errorString();
    Errors::AuthorizationError error();
public Q_SLOTS:
    void refreshSession(Session *session);
private Q_SLOTS:
    void loadFinished(const bool& finished);
    void waitForUserApproval();
    void raiseError(Errors::AuthorizationError errorType, const QString& description);
Q_SIGNALS:
    void pageApproved();
    void finished();

};

#endif // GDRIVELOGINPAGE_H
