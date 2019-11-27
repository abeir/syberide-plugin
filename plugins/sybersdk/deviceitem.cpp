#include "deviceitem.h"

DeviceItem::DeviceItem(QObject *parent) :
    QObject(parent)
{

}

QString DeviceItem::deviceId() const
{
    return m_strDeviceId;
}

void DeviceItem::setDeviceId(const QString &devId)
{
    if(m_strDeviceId ==  devId)
        return;

    m_strDeviceId = devId;
    emit deviceIdChanged();
}

QString DeviceItem::appId() const
{
    return m_strAppId;
}

void DeviceItem::setAppId(const QString &appId)
{
    if(m_strAppId == appId)
        return;

    m_strAppId = appId;
    emit appIdChanged();
}

QString DeviceItem::expTime() const
{
    return m_strExpTime;
}

void DeviceItem::setExpTime(const QString &time)
{
    if(m_strExpTime == time)
        return;

    m_strExpTime = time;
    emit expTimeChanged();
}
