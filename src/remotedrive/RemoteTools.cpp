#include "RemoteTools.h"

RemoteTools::RemoteTools()
{

}

bool RemoteTools::setLastModificationDate(const QString& fileName, const QDateTime& newTime)
{
  Q_ASSERT(QFile(fileName).exists());
  if (newTime.isNull()) {
    return false;
  }
  Q_ASSERT(newTime.isValid());
  // Only Linux supported currently.
  // TODO add support for OSX and windows
  QScopedPointer<utimbuf> buf(new utimbuf());
  buf->actime =  newTime.toLocalTime().toTime_t();
  buf->modtime =  newTime.toLocalTime().toTime_t();
  bool result = utime(fileName.toUtf8(), buf.data()) == 0 ? true : false;
  // file date sync is not critical functionality. It will just cause some upload/download overhead during sync
  // thus should not fail here but just warn user
  if (!result) {
    qDebug() << "Failed to set last modification date to " + newTime.toString();
  }
  return result;

}
