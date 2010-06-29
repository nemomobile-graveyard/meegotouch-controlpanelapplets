/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */
#ifndef ABOUTBUSINESSLOGIC_H
#define ABOUTBUSINESSLOGIC_H

#include <QPointer>
#include <QObject>
#include <QMap>
#include <QDBusError>
#include <QDBusInterface>
#include <PhoneInfo>

class QDBusObjectPath;
class QString;

using namespace Cellular;

/*!
 * To test this class under scratchbox1 I had to start the bluetooth daemon:
 * # source /tmp/session_bus_address.user
 * # bluetoothd -n	
 *
 * This class has some code that is commented out. Please leave these there,
 * when i got information about the IMEI number I will finalize the code.
 */
class AboutBusinessLogic : public QObject
{
    Q_OBJECT

public:
    AboutBusinessLogic ();
    ~AboutBusinessLogic ();

    void initiateDataCollection();

    QString osName ();
    QString osVersion ();
    QString WiFiAddress (const char *iface);
    QString WiFiAddress ();
    QString BluetoothAddress ();
    QString IMEI ();

signals:
    void ready();
    
public slots:
    void defaultBluetoothAdapterAddressReceived (
            QMap<QString, QVariant> properties);
    void defaultBluetoothAdapterReceived (QDBusObjectPath adapter);
    void DBusMessagingFailure (QDBusError error);

private:
    void initiateBluetoothQueries ();
    void initiatePhoneQueries ();

private:
    QPointer<QDBusInterface> m_ManagerDBusIf;
    QPointer<QDBusInterface> m_AdapterDBusIf;
    QPointer<PhoneInfo>      m_PhoneInfo;
    
    bool          m_gotBluetoothAddress;
    QString       m_BluetoothAddress;

    bool          m_gotImei;
    QString       m_Imei;

    QString       m_WifiAddress;
    QString       m_OsName;
    QString       m_OsVersion;
    #ifdef UNIT_TEST
    friend class Ut_AboutBusinessLogic;
    friend class Ut_AboutApplet;
    #endif
};

#endif
