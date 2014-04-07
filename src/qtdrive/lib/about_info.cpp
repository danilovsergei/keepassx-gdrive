#include "about_info.h"

namespace GoogleDrive
{

class AboutInfoPrivate
{
public:
    QVariantMap data;
};

AboutInfo::AboutInfo()
    : d_ptr(new AboutInfoPrivate)
{
}

AboutInfo::AboutInfo(const QVariantMap &data)
    : d_ptr(new AboutInfoPrivate)
{
    d_ptr->data = data;
}

AboutInfo::AboutInfo(const AboutInfo &src)
    : d_ptr(new AboutInfoPrivate)
{
    d_ptr->data = src.d_ptr->data;
}

AboutInfo::~AboutInfo()
{
    delete d_ptr;
}

AboutInfo& AboutInfo::operator=(const AboutInfo& src)
{
    d_ptr->data = src.d_ptr->data;
    return *this;
}

const QVariantMap &AboutInfo::rawData() const
{
    Q_D(const AboutInfo);
    return d->data;
}

QString AboutInfo::name() const
{
    Q_D(const AboutInfo);
    return d->data.value("name").toString();
}

QString AboutInfo::rootFolderId() const
{
    Q_D(const AboutInfo);
    return d->data.value("rootFolderId").toString();
}

quint64 AboutInfo::quotaBytesTotal() const
{
    Q_D(const AboutInfo);
    return d->data.value("quotaBytesTotal").toString().toULongLong();
}

quint64 AboutInfo::quotaBytesUsed() const
{
    Q_D(const AboutInfo);
    return d->data.value("quotaBytesUsed").toString().toULongLong();
}

quint64 AboutInfo::quotaBytesUsedInTrash() const
{
    Q_D(const AboutInfo);
    return d->data.value("quotaBytesUsedInTrash").toString().toULongLong();
}

int AboutInfo::largestChangeId() const
{
    Q_D(const AboutInfo);
    return d->data.value("largestChangeId").toString().toInt();
}

QString AboutInfo::domainSharingPolicy() const
{
    Q_D(const AboutInfo);
    return d->data.value("domainSharingPolicy").toString();
}

QString AboutInfo::permissionId() const
{
    Q_D(const AboutInfo);
    return d->data.value("permissionId").toString();
}

bool AboutInfo::isCurrentAppInstalled() const
{
    Q_D(const AboutInfo);
    return d->data.value("isCurrentAppInstalled").toBool();
}

}
