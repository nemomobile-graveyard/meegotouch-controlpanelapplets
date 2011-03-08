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
#include "offlinebrief.h"

#include <DcpWidgetTypes>
#include <MApplication>
#include <MBanner>
#include <MMessageBox>
#include <MLabel>

#define DEBUG
#include "../debug.h"

#ifdef HAVE_QMSYSTEM
OfflineBrief::OfflineBrief () :
    m_DevMode (new QmDeviceMode()),
    m_infoBanner (0)
{
    connect(m_DevMode, SIGNAL(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode)),
            this, SLOT(devModeChanged(MeeGo::QmDeviceMode::DeviceMode)));
    m_LastMode = m_DevMode->getMode();
}
#else
OfflineBrief::OfflineBrief (),
    m_infoBanner (0)
{
    /*
     * FIXME: To install a version that works without the help of the QmSystem
     * library.
     */
}
#endif

OfflineBrief::~OfflineBrief()
{
    #ifdef HAVE_QMSYSTEM
    delete m_DevMode;
    #endif
}

#ifdef HAVE_QMSYSTEM
void 
OfflineBrief::devModeChanged (
        MeeGo::QmDeviceMode::DeviceMode mode)
{
    SYS_DEBUG("newmode %d", mode);
    m_LastMode = mode;
    emit valuesChanged();
}
#endif

QString
OfflineBrief::titleText () const
{
    //% "Flight mode"
    return qtTrId ("qtn_sett_main_flightmode");
}

QString
OfflineBrief::valueText() const
{
#if 0
    return currentText();
#endif

    return "";
}

QString 
OfflineBrief::currentText() const
{
    #ifdef HAVE_QMSYSTEM
    switch (m_LastMode)
    {
        case QmDeviceMode::Flight:
            //% "Deactivate offline mode"
            return qtTrId("qtn_offl_deactivate");
        case QmDeviceMode::Normal:
        default:
            //% "Activate offline mode"
            return qtTrId("qtn_offl_activate");
    }
    #endif

    return QString("No QmSystem");
}

bool
OfflineBrief::toggle () const
{
    return (m_LastMode == QmDeviceMode::Flight);
}

void 
OfflineBrief::setToggle (
        bool toggle)
{
    SYS_DEBUG ("toggle = %s", SYS_BOOL (toggle));
    Q_UNUSED(toggle);

#ifdef HAVE_QMSYSTEM
    SYS_DEBUG("");
    if (m_LastMode == QmDeviceMode::Flight)
    {
        bool success = m_DevMode->setMode (QmDeviceMode::Normal);
        SYS_DEBUG ("m_DevMode->>setMode (Normal) success: %s", SYS_BOOL (success));
#if 0
        //% "Exit offline mode?"
        MMessageBox* dialog = new MMessageBox("", qtTrId("qtn_offl_exiting"),
            M::YesButton | M::NoButton);
        connect(dialog, SIGNAL(disappeared()), this, SLOT(processDialogResult()));
        dialog->appear(MApplication::activeWindow ());
#endif
    }
    else
    {
        bool success = m_DevMode->setMode (QmDeviceMode::Flight);
        SYS_DEBUG ("m_DevMode->>setMode (Flight) success: %s", SYS_BOOL (success));
        if (success)
        {
            if (! m_infoBanner)
            {
                m_infoBanner = new MBanner;
                m_infoBanner->setStyleName ("InformationBanner");
                m_infoBanner->setObjectName ("InfoBanner");
            }

            //% "Closing all connections. Switching to offline mode."
            m_infoBanner->setTitle (qtTrId ("qtn_offl_entering"));
            m_infoBanner->appear (MApplication::activeWindow ());
        }
    }
#endif
}

void OfflineBrief::processDialogResult()
{
#ifdef HAVE_QMSYSTEM
    MMessageBox *dialog = static_cast<MMessageBox*>(sender());
    if(dialog->result() == MDialog::Accepted)
    {
        m_DevMode->setMode(QmDeviceMode::Normal);
    }
#endif
}

int OfflineBrief::widgetTypeID() const
{
    SYS_DEBUG("");
    return DcpWidgetType::Toggle;
}

