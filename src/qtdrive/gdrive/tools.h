
#ifndef GDRIVE_TOOLS_H
#define GDRIVE_TOOLS_H

#include <QtCore/QCoreApplication>

class finilizer: public QObject
{
    Q_OBJECT
public:
    explicit finilizer(QObject* parent = 0) : QObject(parent) {}
    
public Q_SLOTS:
    void exit(int code) {
        qApp->exit(code);
    }
};

#endif

