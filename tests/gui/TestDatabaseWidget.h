#ifndef TESTDATABASEWIDGET_H
#define TESTDATABASEWIDGET_H
#include <QtCore/QObject>
#include "config-keepassx-tests.h"
#include "tests.h"
#include "core/Config.h"
#include "core/Database.h"
#include "core/Entry.h"
#include "core/Group.h"
#include "core/Metadata.h"
#include "core/Tools.h"
#include "crypto/Crypto.h"
#include "gui/MainWindow.h"
#include "gui/DatabaseTabWidget.h"
#include "gui/DatabaseWidget.h"
#include <QtCore/QTemporaryFile>
#include <QtTest/QTest>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QToolBar>
#include <QtGui/QToolButton>
#include "gui/FileDialog.h"
#include <QtTest/QSignalSpy>

class TestDatabaseWidget : public QObject
{
Q_OBJECT

public:
    TestDatabaseWidget();
private Q_SLOTS:
    void initTestCase();
    /**
     * @brief testModifiedCalledAfterSync tests modified property called after database sync
     */
    void testModifiedCalledAfterSync();
private:
    MainWindow* m_mainWindow;
    DatabaseTabWidget* m_tabWidget;
    DatabaseWidget* m_dbWidget;
    QString m_tmpFileName;
    Database* m_db;
    void triggerAction(const QString& name);
};

#endif // TESTDATABASEWIDGET_H
