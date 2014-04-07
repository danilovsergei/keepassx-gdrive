#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QSet>
#include <QNetworkAccessManager>

#include "../lib/session.h"
#include "../lib/command.h"
#include "../lib/file_info.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private Q_SLOTS:
    void started(GoogleDrive::Command* cmd);
    void finished(GoogleDrive::Command* cmd);

    void showOptionsDialog();
    void login();

    void about();
    void aboutFinished();

    void getFileList();
    void getFileInfo();

    void downloadFile();
    void downloadProgress(qint64 v, qint64 total);
    
    void uploadSimpleFile();
    void uploadSimpleFileFinished();
    void uploadSimpleProgress(qint64 v, qint64 total);

    void deleteFile();
    void updateFile();

private:
    void writeInfo(const QString& msg, bool time = true);
    void writeHint(const QString& msg, bool time = true);
    void writeError(const QString& msg, bool time = true);
    void writeDelimiter();
    void writeText(const QString& msg);
    void writeMessage(const QString& msg, QColor col, bool time);
    void updateStatusBar();

    GoogleDrive::FileInfo getTestFileInfo() const;

    Ui::MainWindow *ui;
    QNetworkAccessManager* manager_;
    GoogleDrive::Session* session_;
    QSet<GoogleDrive::Command*> executingCommands_;
};

#endif // MAIN_WINDOW_H
