#ifndef REMOTETOOLS_H
#define REMOTETOOLS_H
#include <QFile>
#include <QDateTime>
#include <utime.h>
#include <QDebug>

class RemoteTools
{
public:
    RemoteTools();
    static bool setLastModificationDate(const QString& fileName, const QDateTime& newTime);
};

#endif // REMOTETOOLS_H
