#ifndef AUTHCREDENTIALS_H
#define AUTHCREDENTIALS_H
#include <QtCore/QVariant>
#include "core/Global.h"
class AuthCredentials : public QObject
{
    Q_OBJECT
public:
    AuthCredentials(QObject *parent);
    virtual void init() = 0;
    // virtual AuthCredentials* newInstance(QObject* parent) = 0;
    QVariantMap *getCredentials();
    const QString getOption(const QString &option);

public Q_SLOTS:
    virtual void update() = 0;

protected:
    QVariantMap *creds = Q_NULLPTR;

public Q_SLOTS:
    void credentialsUpdated();

Q_SIGNALS:
    void updated();
};

#endif // AUTHCREDENTIALS_H
