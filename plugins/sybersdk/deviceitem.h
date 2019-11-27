#ifndef DEVICEITEM_H
#define DEVICEITEM_H

#include <QObject>
#include <QDateTime>

class DeviceItem : public QObject
{
    Q_OBJECT
public:
    explicit DeviceItem(QObject *parent = 0);
    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(QString appId READ appId WRITE setAppId NOTIFY appIdChanged)
    Q_PROPERTY(QString expTime READ expTime WRITE setExpTime NOTIFY expTimeChanged)

    QString deviceId() const;
    void setDeviceId(const QString &devId);

    QString appId() const;
    void setAppId(const QString &appId);

    QString expTime() const;
    void setExpTime(const QString &time);

signals:
    void deviceIdChanged();
    void appIdChanged();
    void expTimeChanged();

private:
    QString m_strDeviceId;
    QString m_strAppId;
    QString m_strExpTime;

};

#endif // DEVICEITEM_H
