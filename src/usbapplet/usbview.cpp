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
#include "usbview.h"

#include <QGraphicsLinearLayout>
#include <MLabel>
#include <MButton>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MButtonGroup>
#include <MNotification>
#include <MContainer>

#undef DEBUG
#include "../debug.h"

#define BUTTON_ALWAYS_ASK   0
#define BUTTON_MASS_STORAGE 1
#define BUTTON_OVI_SUITE    2

#ifdef HAVE_QMSYSTEM
UsbView::UsbView (MeeGo::QmUSBMode *logic) :
    m_logic (logic),
    m_error (0)
{
    setObjectName ("UsbView");
    initWidget ();
}
#else
/*
 * FIXME: to implement a variant that does not use the QmSystem library.
 */
UsbView::UsbView (void *logic) :
    m_error (0)
{
    setObjectName ("UsbView");
    initWidget ();
}
#endif

UsbView::~UsbView ()
{
    delete m_btn_group;

    if (m_error != 0)
    {
        m_error->remove ();
        delete m_error;
        m_error = 0;
    }
}

void
UsbView::initWidget ()
{
#ifdef HAVE_QMSYSTEM
    MLayout                 *mainLayout;
    MLinearLayoutPolicy     *mainPolicy;

    MLayout                 *buttonsLayout;
    MLinearLayoutPolicy     *portraitButtonsPolicy;
    MLinearLayoutPolicy     *landscapeButtonsPolicy;

    setObjectName ("CommonPanelInverted");

// Creating the main layout
    mainLayout = new MLayout;
    mainPolicy = new MLinearLayoutPolicy (mainLayout, Qt::Vertical);

    mainLayout->setContentsMargins (0. , 0. , 0. , 0.);
    mainPolicy->setSpacing (0. );

// Create the title-bar
    MLabel *title =
        addTitleLabel (mainPolicy,
                       "CommonXLargeHeaderPanelInverted",
                       "CommonXLargeHeaderInverted");
    //% "USB"
    title->setText (qtTrId ("qtn_usb_title"));

// Info label
    m_info_label = addTitleLabel (mainPolicy,
                                  "CommonSmallPanelInverted",
                                  "CommonHeaderInverted");

// Creating, filling and adding the mode-list
    m_btn_group = new MButtonGroup;
    m_btn_group->setExclusive (true);

    buttonsLayout = new MLayout;
    buttonsLayout->setContentsMargins (0, 0, 0, 0);

    portraitButtonsPolicy = new MLinearLayoutPolicy (buttonsLayout, Qt::Vertical);
    portraitButtonsPolicy->setNotifyWidgetsOfLayoutPositionEnabled (true);
    portraitButtonsPolicy->setSpacing (0.);

    buttonsLayout->setPortraitPolicy (portraitButtonsPolicy);

    landscapeButtonsPolicy = new MLinearLayoutPolicy (buttonsLayout, Qt::Horizontal);
    landscapeButtonsPolicy->setNotifyWidgetsOfLayoutPositionEnabled (true);
    landscapeButtonsPolicy->setSpacing (0.);

    buttonsLayout->setLandscapePolicy (landscapeButtonsPolicy);

    for (int i = 0; i < 3; i++)
    {
        int id;
        m_buttons[i] = new MButton;
/*  XXX: this breaks the styling:
 *      m_buttons[i]->setViewType(MButton::groupType);
 */
        m_buttons[i]->setCheckable (true);

        portraitButtonsPolicy->addItem (m_buttons[i]);
        landscapeButtonsPolicy->addItem (m_buttons[i]);

        switch (i)
        {
        case BUTTON_ALWAYS_ASK:
            m_buttons[i]->setObjectName("AlwaysAsk");
            m_buttons[i]->setStyleName ("CommonTopSplitButtonInverted");
            id = (int) QmUSBMode::Ask;
            break;
        case BUTTON_MASS_STORAGE:
            m_buttons[i]->setObjectName("MassStorage");
            m_buttons[i]->setStyleName ("CommonVerticalCenterSplitButtonInverted");
            id = (int) QmUSBMode::MassStorage;
            break;
        case BUTTON_OVI_SUITE:
            m_buttons[i]->setObjectName("OviSuite");
            m_buttons[i]->setStyleName ("CommonBottomSplitButtonInverted");
            id = (int) QmUSBMode::OviSuite;
            break;
        default:
            break;
        }

        m_btn_group->addButton (m_buttons[i], id);
    }

    MContainer *buttonsWidget = new MContainer;
    buttonsWidget->setHeaderVisible (false);
    buttonsWidget->setStyleName ("CommonPanelInverted");

    buttonsWidget->centralWidget ()->setLayout (buttonsLayout);
    mainPolicy->addItem (buttonsWidget);
    mainPolicy->setAlignment (buttonsWidget, Qt::AlignCenter);

    // init the button-group value & connect to its signal
    int current_setting = (int) m_logic->getDefaultMode ();

    if (m_btn_group->button (current_setting) == 0)
        m_btn_group->button ((int) QmUSBMode::Ask)->setChecked (true);
    else
        m_btn_group->button (current_setting)->setChecked (true);

    connect (m_btn_group, SIGNAL (buttonClicked (int)),
             this, SLOT (selectionChanged (int)));

    //finally add a stretch & set the layout
    mainPolicy->addStretch ();

    setLayout (mainLayout);

    retranslateUi ();
#endif
}

void
UsbView::selectionChanged (int id)
{
    #ifdef HAVE_QMSYSTEM
    QmUSBMode::Mode newmode = (QmUSBMode::Mode) id;
    QmUSBMode::Mode active = m_logic->getMode ();

    /*
     * Do nothing if we just tapped on the
     * currently selected one...
     */
    if ((int) m_logic->getDefaultMode() == id)
        return;

    /*
     * First remove the old error notification
     */
    if (m_error != 0)
    {
        m_error->remove ();
        delete m_error;
        m_error = 0;
    }

    /*
     * If we are connected and some mode active, then
     * show an error message and set the mode back
     * to original
     */
    if ((active == QmUSBMode::MassStorage) ||
        (active == QmUSBMode::OviSuite))
    {
        m_btn_group->blockSignals (true);

        /*
         * Set checked on the previously active button
         */
        int current_setting = (int) m_logic->getDefaultMode ();

        if (m_btn_group->button (current_setting) == 0)
            m_btn_group->button ((int) QmUSBMode::Ask)->setChecked (true);
        else
            m_btn_group->button (current_setting)->setChecked (true);

        m_btn_group->blockSignals (false);

        /*
         * Create the error notification
         */
        m_error = new MNotification (
            MNotification::DeviceErrorEvent,
//% "You cannot change USB mode while USB is connecting.<br/>Eject USB device first, and then change setting."
            "", qtTrId ("qtn_usb_change_incorrect"));

        /*
         * And show it
         */
        m_error->publish ();

        return;
    }

    m_logic->setDefaultMode (newmode);

    /*
     * If we are connected, and we've changed the default
     * mode lets activate the selected mode...
     */
    if (m_logic->getMode () == QmUSBMode::ChargingOnly)
        m_logic->setMode (newmode);

    SYS_DEBUG ("emit settingsChanged ()");
    emit settingsChanged ();
    #endif
}

void
UsbView::retranslateUi ()
{
    //% "Default USB device mode"
    m_info_label->setText (qtTrId ("qtn_usb_default_info"));

    //% "Always ask"
    m_buttons[BUTTON_ALWAYS_ASK]->setText (qtTrId ("qtn_usb_always_ask"));
    //% "Mass Storage mode"
    m_buttons[BUTTON_MASS_STORAGE]->setText (qtTrId ("qtn_usb_mass_storage"));
    //% "Ovi Suite mode"
    m_buttons[BUTTON_OVI_SUITE]->setText (qtTrId ("qtn_usb_ovi_suite"));
}

MLabel *
UsbView::addTitleLabel (
    MLinearLayoutPolicy     *targetPolicy,
    const char              *panelStyleName,
    const char              *labelStyleName)
{
    MLabel                  *label;
    MContainer              *container;
    QGraphicsLinearLayout   *containerLayout;

    label = new MLabel;
    label->setStyleName (labelStyleName);

    container = new MContainer;
    container->setStyleName (panelStyleName);
    container->setContentsMargins (0., 0., 0., 0.);
    container->setHeaderVisible (false);

    containerLayout = new QGraphicsLinearLayout (Qt::Horizontal);
    containerLayout->setContentsMargins (0., 0., 0., 0.);
    containerLayout->addItem (label);

    container->centralWidget ()->setLayout (containerLayout);

    targetPolicy->addItem (container);
    return label;
}

