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
#include "wallpaperviewwidget.h"

#include <QTimer>
#include <MWindow>
#include <MApplication>
#include <MApplicationPage>
#include <MPannableViewport>
#include <MPositionIndicator>

#define DEBUG
#define WARNING
#include "../debug.h"

#include "mwidgetcreator.h"
M_REGISTER_WIDGET_NO_CREATE(WallpaperViewWidget)

/******************************************************************************
 *
 */
WallpaperViewWidget::WallpaperViewWidget (
        WallpaperBusinessLogic *wallpaperBusinessLogic, 
        QGraphicsWidget        *parent) :
    DcpStylableWidget (parent),
    m_BusinessLogic (wallpaperBusinessLogic),
    m_BgColor ("black"),
    m_PageRealized (false)
{
    //m_BgColor = style()->imageBackgroundColor();
    #if 0
    MWindow *win = MApplication::activeWindow ();
    if (win)
        win->showFullScreen();
    #endif
   /*
    *
    */
    QTimer::singleShot (0, this, SLOT(createContent()));
}

void
WallpaperViewWidget::saveImage ()
{
    if (m_Trans.expectedSize() != m_OriginalSize) {
        SYS_WARNING ("The original size is not %dx%d",
                m_Trans.expectedSize().width(), 
                m_Trans.expectedSize().height());
        QPixmap pixmap = generatePixmap (
                m_Trans.expectedSize(),
                m_Trans.offset(),
                m_Trans.scale());
        m_BusinessLogic->setWallpaper (pixmap);
    } else {
        m_BusinessLogic->setWallpaper ();
    }

    /*
     * Notifying the business logic about the editing ended. It is important,
     * otherwise the businesslogic will reject the next edit start requests.
     */
    m_BusinessLogic->endEdit ();
}

void
WallpaperViewWidget::dropImage ()
{
    /*
     * Notifying the business logic about the editing ended. It is important,
     * otherwise the businesslogic will reject the next edit start requests.
     */
    m_BusinessLogic->endEdit ();    
}

void
WallpaperViewWidget::createContent ()
{
    MWindow             *win = MApplication::activeWindow ();
    WallpaperDescriptor  desc = m_BusinessLogic->editedImage ();
    QSize                sceneSize;
    int                  xMarg = 0;
    int                  yMarg = 0;

    SYS_DEBUG ("");
   
    if (desc.isNull()) {
        SYS_WARNING ("No editted image, giving up.");
        return;
    }
    

    if (win) {
        sceneSize = win->visibleSceneSize (M::Portrait);
        m_Trans.setExpectedSize (sceneSize);
        m_Trans.setOrientation (M::Portrait);
    }
    
    m_Image = desc.load (m_Trans.expectedSize(), m_OriginalSize);


    if (sceneSize.width() > m_Image.width())
        xMarg = (sceneSize.width() - m_Image.width()) / 2;
    if (sceneSize.height() > m_Image.height())
        yMarg = (sceneSize.height() - m_Image.height()) / 2;

    m_Trans.setOffset (QPointF(xMarg, yMarg));
    this->setMinimumSize (m_Trans.expectedSize());
    
    SYS_DEBUG ("sceneSize = %dx%d", sceneSize.width(), sceneSize.height());
    SYS_DEBUG ("imageSize = %dx%d", m_Image.width(), m_Image.height());
    SYS_DEBUG ("offset    = %d, %d", xMarg, yMarg);
}

bool 
WallpaperViewWidget::pagePans () const
{
    return false;
}

void
WallpaperViewWidget::paint (
        QPainter                          *painter,
        const QStyleOptionGraphicsItem    *option,
        QWidget                           *widget)
{
    MWidget::paint (painter, option, widget);
    
    painter->fillRect (
            0, 0, 
            m_Trans.expectedWidth (),
            m_Trans.expectedHeight (),
            m_BgColor);

    painter->drawImage (
            QRect (imageX(), imageY(), imageDX(), imageDY()),
            m_Image);
}

void
WallpaperViewWidget::polishEvent ()
{
    SYS_DEBUG ("");
    QGraphicsWidget  *parent;
    MApplicationPage *page = 0;

    /*
     * Just a protection about double adding the actions.
     */
    if (m_PageRealized)
        return;
    
    /*
     * We need to find the MApplicationPage among our parents.
     */
    parent = parentWidget();
    while (parent) {
        page = qobject_cast <MApplicationPage *>(parent);
        if (page)
            break;
        parent = parent->parentWidget();
    }
    m_PageRealized = true;

    if (!page)
        return;

    page->setPannable (false);
    page->pannableViewport()->positionIndicator()->hide();
    /**************************************************************************
     * Hiding the home button and the escape button from the page. 
     */
    page->setComponentsDisplayMode (
            MApplicationPage::EscapeButton,
            MApplicationPageModel::Hide);
    page->setComponentsDisplayMode (
            MApplicationPage::HomeButton,
            MApplicationPageModel::Hide);

    //% "Save"
    m_DoneAction = new MAction(qtTrId("qtn_comm_save"), this);
    m_DoneAction->setLocation(MAction::ToolBarLocation);
    page->addAction(m_DoneAction);
    connect(m_DoneAction, SIGNAL(triggered()), this, SLOT(slotDoneActivated()));

    //% "Cancel"
    m_CancelAction = new MAction(qtTrId("qtn_comm_cancel"), this);
    m_CancelAction->setLocation(MAction::ToolBarLocation);
    page->addAction(m_CancelAction);
    connect(m_CancelAction, SIGNAL(triggered()), this, SLOT(slotCancelActivated()));
}

int
WallpaperViewWidget::imageX () const
{
    int retval = 0;

    retval += m_Trans.x();
    return retval;
}

int
WallpaperViewWidget::imageY () const
{
    int retval = 0;

    retval += m_Trans.y();

    // FIXME: testing
    //retval -= 110;
    return retval;
}

int
WallpaperViewWidget::imageDX () const
{
    return m_Trans.scale() * m_Image.width();
}

int
WallpaperViewWidget::imageDY () const
{
    return m_Trans.scale() * m_Image.height();
}

QPixmap
WallpaperViewWidget::generatePixmap (
        const QSize    &expectedSize,
        const QPointF  &offset,
        qreal           scale)
{
    QPixmap   retval (expectedSize);
    QPainter  painter (&retval);
    QRectF    area (offset.x(), offset.y(), 
                    (scale * m_Image.width ()),
                    (scale * m_Image.height ()));

    retval.fill (m_BgColor);
    painter.drawImage (area, m_Image);

    return retval;
}
