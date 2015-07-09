#include "AuthCredentials.h"

AuthCredentials::AuthCredentials(QObject *parent) : QObject(parent)
{
}

void AuthCredentials::credentialsUpdated() {

}


QVariantMap AuthCredentials::getCredentials()
{
    return creds;
}

const QString AuthCredentials::getOption(const QString &option)
{
    Q_ASSERT(creds.contains(option));
    return creds.value(option).toString();
}
