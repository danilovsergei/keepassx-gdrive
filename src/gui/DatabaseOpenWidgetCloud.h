#ifndef DATABASEOPENWIDGETCLOUD_H
#define DATABASEOPENWIDGETCLOUD_H

#include "gui/DatabaseWidget.h"
#include "gui/DialogyWidget.h"
#include "QtCore/QList"
#include "remotedrive/gdrive/GoogleDriveTools.h"
#include "remotedrive/RemoteDriveApi.h"
#include "remotedrive/gdrive/CommandsFactoryImpl.h"
#include "remotedrive/gdrive/GoogleDriveCredentials.h"
#include "remotedrive/Errors.h"
#include "remotedrive/OptionsBuilder.h"
#include "remotedrive/RemoteFile.h"
namespace Ui {
class DatabaseOpenWidgetCloud;
}

class DatabaseOpenWidgetCloud : public DialogyWidget
{
    Q_OBJECT

Q_SIGNALS:
    void editFinished(bool accepted);
    void widgetLoaded();
    void cloudSelected();
    void dbSelected(QString);
    void dbRejected();

protected Q_SLOTS:
    void selectCloud(int cloud);
    void cloudDbLoad();
    void downloadDb();
    void reject();

public Q_SLOTS:
    void loadSupportedCloudEngines();


public:
    explicit DatabaseOpenWidgetCloud(QWidget *parent = 0);
    ~DatabaseOpenWidgetCloud();

private:
    Ui::DatabaseOpenWidgetCloud *ui;
    RemoteDriveApi* remoteDrive = Q_NULLPTR;
    KeePassxDriveSync::Command downloadCommand;
    KeePassxDriveSync::Command listCommand;
private Q_SLOTS:
    void downloadDbFinished();
};

#endif // DATABASEOPENWIDGETCLOUD_H
