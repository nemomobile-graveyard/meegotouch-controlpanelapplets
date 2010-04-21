/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */
#ifndef ABOUTBUSINESSLOGIC_H
#define ABOUTBUSINESSLOGIC_H

#include <QPointer>
#include <QObject>
#include <QMap>
#include <QDBusError>
#include <QDBusInterface>

class QDBusObjectPath;
class QString;


/*!
 * To test this class under scratchbox1 I had to start the bluetooth daemon:
 * # source /tmp/session_bus_address.user
 * # bluetoothd -n
 */
class AboutBusinessLogic : public QObject
{
    Q_OBJECT

public:
    AboutBusinessLogic ();
    ~AboutBusinessLogic ();

    void initiateBluetoothQueries ();
    void initiatePhoneQueries ();

    QString osVersion ();
    QString WiFiAddress (const char *iface);
    QString WiFiAddress ();
    QString BluetoothAddress ();
    QString IMEI ();

public slots:
    void defaultBluetoothAdapterAddressReceived (
            QMap<QString, QVariant> properties);
    void defaultBluetoothAdapterReceived (QDBusObjectPath adapter);
    void imeiReceived (QString imei);
    void DBusMessagingFailure (QDBusError error);

private:
    QPointer<QDBusInterface> m_ManagerDBusIf;
    QPointer<QDBusInterface> m_AdapterDBusIf;
    QPointer<QDBusInterface> m_ImeiDBusIf;
    bool          m_gotBluetoothAddress;
    QString       m_BluetoothAddress;

    bool          m_gotImei;
    QString       m_Imei;
};

#endif