#include "TestDatabaseWidget.h"

TestDatabaseWidget::TestDatabaseWidget()
{
}
void TestDatabaseWidget::initTestCase()
{
    Crypto::init();
    Config::createTempFileInstance();
    m_mainWindow = new MainWindow();
    m_tabWidget = m_mainWindow->findChild<DatabaseTabWidget*>("tabWidget");
    m_mainWindow->show();
    Tools::wait(50);
}

void TestDatabaseWidget::testModifiedCalledAfterSync(){
    fileDialog()->setNextFileName(QString(KEEPASSX_TEST_DATA_DIR).append("/NewDatabase.kdbx"));
    triggerAction("actionDatabaseOpen");

    QWidget* databaseOpenWidget = m_mainWindow->findChild<QWidget*>("databaseOpenWidget");
    QLineEdit* editPassword = databaseOpenWidget->findChild<QLineEdit*>("editPassword");
    QVERIFY(editPassword);

    QTest::keyClicks(editPassword, "a");
    QTest::keyClick(editPassword, Qt::Key_Enter);
    QCOMPARE(m_tabWidget->count(), 1);
    QCOMPARE(m_tabWidget->tabText(m_tabWidget->currentIndex()), QString("NewDatabase.kdbx"));

    m_dbWidget = m_tabWidget->currentDatabaseWidget();
    m_db = m_dbWidget->database();
    triggerAction("actionSync");
    QAction* action = m_mainWindow->findChild<QAction*>("actionSync");
    QSignalSpy spy(action, SIGNAL(triggered()));
    int waitTime = 0;

    while (spy.count() == 0 && waitTime < 10000) {
      QTest::qWait(200);
      waitTime += 200;
    }

}

void TestDatabaseWidget::triggerAction(const QString& name)
{
    QAction* action = m_mainWindow->findChild<QAction*>(name);
    QVERIFY(action);
    QVERIFY(action->isEnabled());
    action->trigger();
}
QTEST_MAIN(TestDatabaseWidget)
