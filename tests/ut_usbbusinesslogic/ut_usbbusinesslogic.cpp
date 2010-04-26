/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et sw=4 ts=4 sts=4: */
#include "ut_usbbusinesslogic.h"
#include "usbbusinesslogic.h"

#include <MApplication>
#include <QList>

#include <usb_moded-modes.h>
#include <usb_moded-dbus.h>

#define DEBUG
#include "../../src/debug.h"

void 
Ut_UsbSettingsLogic::init()
{
}

void 
Ut_UsbSettingsLogic::cleanup()
{
}

usb_modes toRestore;

void 
Ut_UsbSettingsLogic::initTestCase()
{
    int argc = 1;
    char* app_name = (char*) "./Ut_UsbSettingsLogic";

    m_App = new MApplication (argc, &app_name);
    m_Api = new UsbSettingsLogic;

    toRestore = m_Api->getUsbSetting ();
}

void 
Ut_UsbSettingsLogic::cleanupTestCase()
{
    m_Api->setUsbSetting (toRestore);

    delete m_App;
    delete m_Api;
}

// Try to save valid settings
// and check the results
void
Ut_UsbSettingsLogic::testValidSaveLoadConfig ()
{
    // Test Ovi Suite mode setting
    m_Api->setUsbSetting (USB_OVI_SUITE);
    QVERIFY (m_Api->getUsbSetting () == USB_OVI_SUITE);

    // Test Mass Storage mode setting
    m_Api->setUsbSetting (USB_MASS_STORAGE);
    QVERIFY (m_Api->getUsbSetting () == USB_MASS_STORAGE);

    // Test Always ask setting
    m_Api->setUsbSetting (USB_AUTO);
    QVERIFY (m_Api->getUsbSetting () == USB_AUTO);
}

void
Ut_UsbSettingsLogic::testInvalidSaveLoadConfig ()
{
    // Set a valid setting
    m_Api->setUsbSetting (USB_OVI_SUITE);

    // Try to set an invalid value
    m_Api->setUsbSetting (USB_NOTCONNECTED);

    QVERIFY (m_Api->getUsbSetting () == USB_OVI_SUITE);


    // NOOP shown as Ask (AUTO) on UI, try to do that:
    m_Api->setUsbSetting (USB_NOOP);

    QVERIFY (m_Api->getUsbSetting () == USB_AUTO);
}

void
Ut_UsbSettingsLogic::testUsbModedRelation ()
{
    QSignalSpy spy (m_Api, SIGNAL (currentModeChanged (usb_modes)));

    // Simulate some usb_moded signals:
    m_Api->usbModeChange (QString (MODE_MASS_STORAGE));
    m_Api->usbModeChange (QString (USB_DISCONNECTED));
    m_Api->usbModeChange (QString (MODE_OVI_SUITE));
    m_Api->usbModeChange (QString (USB_CONNECTED));

    QTest::qWait (100);

    QVERIFY (spy.count () == 4);

    // Do some checks here...
    QList<QVariant> arguments = spy.takeAt (0);
    usb_modes value = arguments.at (0).value<usb_modes> ();
    QVERIFY (value == USB_MASS_STORAGE);

    arguments = spy.takeAt (0);
    value = arguments.at (0).value<usb_modes> ();
    QVERIFY (value == USB_NOTCONNECTED);

    arguments = spy.takeAt (0);
    value = arguments.at (0).value<usb_modes> ();
    QVERIFY (value == USB_OVI_SUITE);

    arguments = spy.takeAt (0);
    value = arguments.at (0).value<usb_modes> ();
    QVERIFY (value == USB_NOOP);
}

QTEST_APPLESS_MAIN(Ut_UsbSettingsLogic)
