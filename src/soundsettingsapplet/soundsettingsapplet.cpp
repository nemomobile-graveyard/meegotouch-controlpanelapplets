/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelsoundsettingsapplet.
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
#include "soundsettingsapplet.h"

#include <gst/gst.h>
#include <MAction>

#include "alerttone.h"
#include "alerttonetoplevel.h"
#include "alerttonebrowser.h"
#include "alerttoneappletwidget.h"

// for freeing up the singletons
#include "alerttonepreview.h"
#include "trackerconnection.h"
#include "profilebackend.h"

#define DEBUG
#define WARNING
#include "../debug.h"

#ifndef UNIT_TEST
#include <MLibrary>
M_LIBRARY
#endif

Q_EXPORT_PLUGIN2(soundsettingsapplet, SoundSettingsApplet)

int    gst_argc = 1;
char** gst_argv = NULL;

SoundSettingsApplet::SoundSettingsApplet()
{
}

SoundSettingsApplet::~SoundSettingsApplet()
{
    gst_deinit ();

    if ((gst_argv != NULL) && (gst_argv[0] != NULL))
    {
        delete[] gst_argv[0];
        delete[] gst_argv;
    }

    // free up the m_alertTones member
    if (! m_alertTones.isEmpty ())
    {
        foreach (AlertTone *tone, m_alertTones)
            delete tone;
        m_alertTones.clear ();
    }

    /*
     * And then free up the singletons
     */
    delete ProfileBackend::getInstance ();
    delete TrackerConnection::instance ();

    AlertTonePreview::freeResources ();

    SysDebug::closeDebugFile ();
}

void
SoundSettingsApplet::init()
{
    SYS_DEBUG ("");
    gst_argv = new char*[2];
    gst_argv[0] = qstrdup ("app");
    gst_argv[1] = NULL;

    gst_init (&gst_argc, &gst_argv);

    m_alertTones = AlertTone::alertTones ();
}

/* widgetId: 0xaaaabbbb where 
   0xaaaa is the widget ID and 
   0xbbbb is the alert tone index */
DcpWidget *
SoundSettingsApplet::constructWidget(int widgetId)
{
    SYS_DEBUG ("%s: widgetId = %d", SYS_TIME_STR, widgetId);
    AlertToneToplevel *newWidget = NULL;
    int realWidgetId = widgetId / 65536;
    int alertToneIdx = widgetId - realWidgetId * 65536;

    if (m_stack.size() > 0)
        if (((m_stack.top()->getWidgetId() / 65536) == AlertToneBrowser_id      && realWidgetId == AlertToneBrowser_id) ||
            ((m_stack.top()->getWidgetId() / 65536) == AlertToneAppletWidget_id && realWidgetId != AlertToneBrowser_id))
            return NULL;

    if (AlertToneAppletWidget_id == realWidgetId)
        newWidget = new AlertToneAppletWidget(m_alertTones);
    else
    if (AlertToneBrowser_id == realWidgetId && alertToneIdx >= 0 && alertToneIdx < m_alertTones.size())
        newWidget = new AlertToneBrowser(m_alertTones[alertToneIdx]);
    else
        SYS_WARNING ("Invalid widgetId = %d", widgetId);

    if (newWidget)
    {
        m_stack.push(newWidget);
        connect (newWidget, SIGNAL (destroyed (QObject *)),
                 SLOT (toplevelDestroyed (QObject *)));
    }

    SYS_DEBUG ("%s: done", SYS_TIME_STR);
    return newWidget;
}

void
SoundSettingsApplet::toplevelDestroyed(QObject *goner)
{
    if (m_stack.size() > 0)
        if (goner == qobject_cast<QObject *>(m_stack.top()))
            m_stack.pop();
}

QString
SoundSettingsApplet::title() const
{
    //% "Alert tones"
    QString title = qtTrId ("qtn_sond_sounds");

    if ((m_stack.size() > 0) && (m_stack.top()))
        title = qobject_cast<AlertToneToplevel *>(m_stack.top())->title();

    return title;
}

QVector<MAction*> 
SoundSettingsApplet::viewMenuItems()
{
    QVector<MAction*> vector;

    if (m_stack.size() > 0)
        if (m_stack.top())
            vector = qobject_cast<AlertToneToplevel *>(m_stack.top())->viewMenuItems();

    return vector;
}

DcpBrief *
SoundSettingsApplet::constructBrief (
        int partId)
{
    Q_UNUSED (partId);
    return NULL;
}

