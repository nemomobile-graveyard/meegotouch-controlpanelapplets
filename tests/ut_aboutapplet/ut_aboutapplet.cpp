/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelapplets.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "ut_aboutapplet.h"
#include "aboutapplet.h"
#include "aboutwidget.h"

#include <dcpstylablewidget.h>
#include <dcpwidgettypes.h>

#include <QSystemInfo>
#include <QSystemDeviceInfo>

QTM_USE_NAMESPACE

#include <mdesktopentry.h>
#include <MApplication>
#include <MAction>
#include <QVector>

#define DEBUG
#include "../../src/debug.h"


/******************************************************************************
 * Ut_AboutApplet implementation.
 */
void
Ut_AboutApplet::init()
{
}

void
Ut_AboutApplet::cleanup()
{
}


static int argc = 1;
static char *app_name = (char*) "./ut_aboutapplet";

void
Ut_AboutApplet::initTestCase()
{
    m_App = new MApplication (argc, &app_name);

    /*
     * We create an AboutApplet here. The applet will create a businesslogic,
     * for which we check that the data collection is not started.
     */
    m_Applet = new AboutApplet;

    QVERIFY (!m_Applet->m_MainWidget);
    m_Applet->init ();

    /*
     * Testing if the widget is not created yet.
     */
    QVERIFY (!m_Applet->m_MainWidget);

#if 0
    /*
     * TODO: FIXME: remove this, but output can be helpful
     * for development & testing purposes
     */
    QSystemDeviceInfo deviceInfo;
    SYS_DEBUG ("\n****** QSystemDeviceInfo *****\n"
               "***** imei: %s\n***** manufacturer: %s\n"
               "***** model: %s\n***** productName: %s\n",
               SYS_STR(deviceInfo.imei ()), SYS_STR(deviceInfo.manufacturer ()),
               SYS_STR(deviceInfo.model ()), SYS_STR(deviceInfo.productName ()));

    QSystemInfo systemInfo;
    SYS_DEBUG ("\n****** QSystemInfo versions *****\n"
               "***** Os: %s\n***** QtCore: %s\n***** Firmware: %s\n",
               SYS_STR(systemInfo.version (QSystemInfo::Os)),
               SYS_STR(systemInfo.version (QSystemInfo::QtCore)),
               SYS_STR(systemInfo.version (QSystemInfo::Firmware)));
#endif
}

void
Ut_AboutApplet::cleanupTestCase()
{
    delete m_Applet;
    m_App->deleteLater ();
}

void
Ut_AboutApplet::testTitle ()
{
    QString title = m_Applet->title();
    QVERIFY (!title.isEmpty());
}

void
Ut_AboutApplet::testConstructWidget ()
{
    AboutWidget     *widget;
    bool             backAccepted;

    /*
     * Testing if the applet creates a widget the first time.
     */
    widget = (AboutWidget *) m_Applet->constructStylableWidget (0);
    QVERIFY (widget);
    QVERIFY (m_Applet->m_MainWidget == widget);
    QVERIFY (!m_Applet->constructStylableWidget (1));

    /* FIXME: TODO check this */
#if 0
    /*
     * logic only should be created, when view is ready
     */
    QVERIFY (m_Applet->m_AboutBusinessLogic);

    /*
     * We are not testing the AboutBusinesslogic here, so the following lines
     * act as a kind of mocking, the aboutbusinesslogic will not initiate
     * outside communication if it has the data already.
     */
    m_Applet->m_AboutBusinessLogic->m_BluetoothAddress = "fakeBluetooth";
    m_Applet->m_AboutBusinessLogic->m_Imei = "FakeImei";
    m_Applet->m_AboutBusinessLogic->m_WifiAddress = "FakeWifi";
    m_Applet->m_AboutBusinessLogic->m_OsName = "FakeOsName";
    m_Applet->m_AboutBusinessLogic->m_OsVersion = "FakeOsVersion";

#endif

    /*
     * Testing if the widget accepts the back. Our applets always accept back.
     */
    backAccepted = widget->back();
    QVERIFY (backAccepted);

    /*
     * Testing if the applet knows about the destruction of the widget.
     */
    delete widget;
    QVERIFY (!m_Applet->m_MainWidget);
}

void
Ut_AboutApplet::testMenuItems ()
{
    QVector<MAction*> items = m_Applet->viewMenuItems ();

    QVERIFY (items.size() == 0);
}

QTEST_APPLESS_MAIN(Ut_AboutApplet)
