#ifndef AUTHCREDENTIALS_H
#define AUTHCREDENTIALS_H
#include <QtCore/QVariant>
class AuthCredentials : public QObject
{
Q_OBJECT
public:
    AuthCredentials();
    virtual void init() = 0;
    virtual void update() = 0;
    virtual AuthCredentials newInstance();
    QVariantMap* getCredentials();

private:
    QVariantMap* creds;

public Q_SLOTS:
    void credentialsUpdated();

Q_SIGNALS:
    void updateCredentials(QVariantMap);

};

#endif // AUTHCREDENTIALS_H
