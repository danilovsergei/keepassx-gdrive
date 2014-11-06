#ifndef GDRIVEUPLOADDBHELPERTEST_H
#define GDRIVEUPLOADDBHELPERTEST_H
#include <QtCore/QObject>
#include <QtTest/QTest>
#include "../tests.h"
#include "gdrive/Errors.h"
#include "format/KeePass2XmlWriter.h"
#include <QtCore/QTemporaryFile>
#include "GDriveTestUtils.h"
#include "crypto/Crypto.h"
class GDriveUploadDbHelperTest: public QObject {
    Q_OBJECT
private Q_SLOTS:
//    void init();
//    void cleanup();
    void initTestCase();
    void testDbUpload();
};

#endif // GDRIVEUPLOADDBHELPERTEST_H
