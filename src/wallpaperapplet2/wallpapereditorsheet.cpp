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
** Contact: Karoliina T. Salminen <karoliina.t.salminen@nokia.com>
** Authors: David Kedves <dkedves@blumsoft.eu>
**          Laszlo Pere <lpere@blumsoft.eu>
**
****************************************************************************/
#include "wallpapereditorsheet.h"
#include "wallpaperbusinesslogic.h"
#include "wallpaperconfiguration.h"
#include "wallpapereditorwidget.h"

#include <MBasicSheetHeader>
#include <QAction>

#include <mwidgetcreator.h>
M_REGISTER_WIDGET_NO_CREATE(WallpaperEditorSheet)


#define DEBUG
#define WARNING
#include "../debug.h"

WallpaperEditorSheet::WallpaperEditorSheet (
        WallpaperBusinessLogic *wallpaperBusinessLogic) :
    m_EditorWidget (0),
    m_Saving (false)
{
    setStyleName ("Inverted");
    createCentralWidget (wallpaperBusinessLogic);
    createHeaderWidget ();
        
    connect (wallpaperBusinessLogic, SIGNAL(wallpaperSaved()),
            this, SLOT(wallpaperSaved()));
    
    connect (wallpaperBusinessLogic, SIGNAL(wallpaperChanged()),
            this, SLOT(cancelActivated()));
}

void 
WallpaperEditorSheet::createCentralWidget(
        WallpaperBusinessLogic *wallpaperBusinessLogic)
{
    m_EditorWidget = new WallpaperViewWidget (wallpaperBusinessLogic);

    SYS_DEBUG ("setCentralWidget(%p)", m_EditorWidget);
    setCentralWidget (m_EditorWidget);
    SYS_DEBUG ("end");
}

void 
WallpaperEditorSheet::createHeaderWidget()
{
    MBasicSheetHeader *basicHeader = new MBasicSheetHeader (this);
    QAction           *doneAction;
    QAction           *cancelAction;

    basicHeader->setStyleName ("Inverted");
    //basicHeader->setProperty ("opacity", 0.2);
    doneAction = new QAction(qtTrId("qtn_comm_command_done"), basicHeader);
    cancelAction = new QAction(qtTrId("qtn_comm_cancel"), basicHeader);
    
    connect(doneAction, SIGNAL(triggered()), SLOT(doneActivated()));
    connect (cancelAction, SIGNAL(triggered()), SLOT(cancelActivated()));

    basicHeader->setPositiveAction (doneAction);
    basicHeader->setNegativeAction (cancelAction);

    setHeaderWidget(basicHeader);
}

void
WallpaperEditorSheet::doneActivated ()
{
    MBasicSheetHeader *header = static_cast<MBasicSheetHeader*>(headerWidget());

    header->setProgressIndicatorVisible (true);
    header->positiveAction()->setEnabled(false);
    header->negativeAction()->setEnabled(false);

    m_Saving = true;

    if (m_EditorWidget)
        m_EditorWidget->saveImage ();
}

void
WallpaperEditorSheet::cancelActivated ()
{
    if (m_EditorWidget)
        m_EditorWidget->dropImage ();

    dismiss ();
}

void
WallpaperEditorSheet::wallpaperSaved ()
{
    if (!m_Saving) {
        SYS_WARNING ("We didn't request saving!");
        return;
    }

    if (m_EditorWidget)
        m_EditorWidget->dropImage ();

    m_Saving = false;
    dismiss ();
}
