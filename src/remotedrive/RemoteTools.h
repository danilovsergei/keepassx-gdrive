#ifndef REMOTETOOLS_H
#define REMOTETOOLS_H
#include <QtCore/QFile>
#include <QtCore/QDateTime>
#include <utime.h>
#include <QtCore/QDebug>

class RemoteTools
{
public:
    RemoteTools();
    static bool setLastModificationDate(const QString& fileName, const QDateTime& newTime);
};

#endif // REMOTETOOLS_H
