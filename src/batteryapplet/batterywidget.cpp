/****************************************************************************+
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
#include "batterywidget.h"
#include "batteryimage.h"
#include "batterybusinesslogic.h"
#include "dcpbattery.h"
#include "slidercontainer.h"
#include "percentagecontainer.h"

#include <QGraphicsLinearLayout>
#include <QTimer>

#include <MStylableWidget>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MButton>
#include <MContainer>
#include <MLabel>
#include <MSlider>
#include <MHelpButton>
#include <MBasicLayoutAnimation>
#include <MComboBox>
#include <MHelpButton>

#include "../styles.h"

#define DEBUG
#define WARNING
#include "../debug.h"

#include <MSeparator>
static const int ActivationContainerPosition = 2;
static const int SpacerPosition = ActivationContainerPosition + 1;
static const int SliderContainerPosition = SpacerPosition + 1;
static const int ConditionContainerPostion = SliderContainerPosition + 1;

/******************************************************************************
 * BatteryWidget implementation.
 */
BatteryWidget::BatteryWidget (QGraphicsWidget *parent) :
        DcpWidget (parent),
        m_logic (0),
        m_MainLayout (0),
        m_RemainingContainer (0),
        m_ActivationContainer (0),
        m_SliderContainer (0),
        m_PSMAutoCombo (0),
        m_BatteryImage (0),
        m_TitleLabel (0),
        m_UILocked (false)
{
    SYS_DEBUG ("Starting in %p", this);

    setContentsMargins (0., 0., 0., 0.);

    // instantiate the batterybusinesslogic
    m_logic = new BatteryBusinessLogic;
    initWidget ();
}

BatteryWidget::~BatteryWidget ()
{
    SYS_DEBUG ("Destroying %p", this);

    if (m_logic) {
        delete m_logic;
        m_logic = 0;
    }
}

bool BatteryWidget::back ()
{
    return true; // back is handled by main window by default
}

void 
BatteryWidget::initWidget ()
{
    MLayout     *layout;
    MSeparator  *spacer;

    /*
     * Creating a layout that holds the rows of the internal widgets.
     */
    layout = new MLayout (this);
    m_MainLayout = new MLinearLayoutPolicy (layout, Qt::Vertical);
    m_MainLayout->setContentsMargins (0., 0., 0., 0.);
    m_MainLayout->setSpacing (0.);
    setLayout (layout);

    /*
     * Adding the rows of widgets.
     */
    // Row 1: The title label
    addHeaderContainer ();
    // Row 2: Remaining capacity widget
    addRemainingCapacityWidget ();
    // Row 3: PSM Auto activation switch
    addAutoActivationWidget ();
    
    spacer = addSpacer (
            "CommonHorizontalSeparatorInverted",
            SpacerPosition);

    addSliderContainer ();

    // Row 4-5: PSM Auto activation text and slider
    // Row 7: Battery condition info.
    addBatteryConditionWidget ();
   

    m_MainLayout->addStretch();

    /*
     * Initializing the wigets and connecting the signals.
     */
    // SliderContainer signals and slots, and initialization
    m_SliderContainer->initSlider (m_logic->PSMThresholdValues ());
    connect (m_SliderContainer, SIGNAL (PSMThresholdValueChanged (int)),
             m_logic, SLOT (setPSMThresholdValue (int)),
             Qt::DirectConnection);

    // connect the value receive signals
    connect (m_logic, SIGNAL(remainingBatteryCapacityChanged(int)),
             this, SLOT(remainingBatteryCapacityReceived(int)));

    // Connect the batteryImage slots.
    connect (m_logic, SIGNAL (batteryCharging (int)),
             m_BatteryImage, SLOT (startCharging (int)));
    connect (m_logic, SIGNAL (batteryBarValueReceived (int)),
             m_BatteryImage, SLOT (updateBatteryLevel (int)));

    // Connecting the signals of the businesslogic.
    connect (m_logic, SIGNAL (PSMValueReceived (bool)),
             this, SLOT (PSMValueReceived (bool)));

    connect (m_logic, SIGNAL (updateUIonPowerSaveModeChange (PowerSaveOpt)),
             this, SLOT (updateUIonPowerSaveModeChange (PowerSaveOpt)));
    connect (m_logic, SIGNAL(batteryFull()),
             m_BatteryImage, SLOT(chargeComplete()));
    connect (m_logic, SIGNAL(batteryFull()),
             this, SLOT(chargeComplete()));
    
    // Connect busines logic
    m_logic->requestValues ();

    /*
     * now initial state of GUI , combobox, labels, slider
     * base on psm state, 
     */
    showHideUi ();
}

void
BatteryWidget::showHideUi ()
{
    if (m_logic->PSMAutoValue ())
    {
        m_PSMAutoCombo->setCurrentIndex((int)PSMAutoAutomatic);
    } else {
        m_PSMAutoCombo->setCurrentIndex((int)m_logic->PSMValue());
    }
    m_logic->setPSMOption((BatteryBusinessLogic::PowerSaveOpt)m_PSMAutoCombo->currentIndex());
}


void 
BatteryWidget::addHeaderContainer ()
{
// In Meego header should be avoided as it has a title bar
#ifndef MEEGO    
    QGraphicsLinearLayout   *layout;
    MHelpButton             *helpButton;

    Q_ASSERT (m_MainLayout);

    layout = new QGraphicsLinearLayout (Qt::Horizontal);
    layout->setContentsMargins (0,0,0,0);

    /*
     * The label that we use as title.
     */
    //% "Battery"
    m_TitleLabel = new MLabel (qtTrId ("qtn_ener_battery"));
    m_TitleLabel->setStyleName (APP_TITLE_LABEL_STYLE_NAME);
    layout->addItem (m_TitleLabel);
    layout->setAlignment (m_TitleLabel, Qt::AlignLeft);

    /*
     * A simple help button to the right of the title label. It is inverted,
     * loads the userguide, so userguide-content have to be installed to see the
     * content.
     */
    helpButton = new MHelpButton("IDUG_MEEGO_BATTERY.html");
    helpButton->setViewType(MButton::iconType);
    helpButton->setIconID ("icon-m-content-description-inverse");
    helpButton->setStyleName ("CommonRightIcon");

    layout->addItem (helpButton);
    layout->setAlignment (helpButton, Qt::AlignVCenter | Qt::AlignRight);

    /*
     * Adding the whole row to the main container.
     */
    m_MainLayout->addItem (layout);
    m_MainLayout->setStretchFactor (layout, 0);
#endif
}

void 
BatteryWidget::addRemainingCapacityWidget ()
{
    Q_ASSERT (m_MainLayout);
    
    m_BatteryImage = new BatteryImage;
    m_BatteryImage->setStyleName ("CommonMainIcon");

    //% "Battery level \%L1\%"
    m_RemainingContainer = new PercentageContainer (
            qtTrId ("qtn_ener_battery_level"), m_BatteryImage);

    m_MainLayout->addItem (m_RemainingContainer);
    m_MainLayout->setStretchFactor (m_RemainingContainer, 0);
    
    connect (m_logic, SIGNAL(batteryCharging (int)),
             this, SLOT(charging(int)));
    m_logic->remainingCapacityRequired();

}

void 
BatteryWidget::addAutoActivationWidget ()
{
    QGraphicsLinearLayout *layout;

    Q_ASSERT (m_MainLayout);
    Q_ASSERT (m_logic);

    /*
     * Creating the container and the layout.
     */
    m_ActivationContainer = new MContainer (this);
    m_ActivationContainer->setContentsMargins (0,0,0,0);

    m_ActivationContainer->setStyleName ("CommonPanelInverted");
    m_ActivationContainer->setHeaderVisible (false);
    layout = new QGraphicsLinearLayout (Qt::Horizontal);
    layout->setContentsMargins (0,0,0,0);
    layout->setSpacing (0.);

    m_ActivationContainer->centralWidget()->setLayout (layout);

    /*
     * A label for the PSM auto activation.
     */
    /*
     * A help button for the PSM auto activation.
     */
#ifdef HAVE_USERGUIDE
    MHelpButton* helpButton = new MHelpButton ("IDUG_MEEGO_BATTERY.html");
    helpButton->setViewType(MButton::iconType);
    helpButton->setIconID ("icon-m-content-description");
#endif    

    /*
     * A combo box choosing the auto PSM mode between on, off and automatic
     */
    m_PSMAutoCombo = new MComboBox();
    //% "Power Save Mode"
    m_PSMAutoCombo->setTitle(qtTrId ("qtn_ener_power_save_combo"));
    m_PSMAutoCombo->setStyleName ("CommonComboBoxInverted");
    m_PSMAutoCombo->setObjectName ("AutoActivatePowerSaveButton");
   
    // See bug 234041
    //% "Off"
    m_PSMAutoCombo->addItem(qtTrId("qtn_comm_settings_off"));
    //% "On"
    m_PSMAutoCombo->addItem(qtTrId("qtn_comm_settings_on"));
    //% "Automatic"
    m_PSMAutoCombo->addItem(qtTrId("qtn_ener_autops"));

   connect (m_PSMAutoCombo, SIGNAL (activated (int)),
             this, SLOT (PSMAutoActivated (int)));

    /*
     * Adding the widgets to the layout.
     */
    #ifdef HAVE_USERGUIDE
    layout->addItem(helpButton);
    #endif
    layout->addItem (m_PSMAutoCombo);
    layout->setAlignment(m_PSMAutoCombo, Qt::AlignRight | Qt::AlignVCenter);

    /*
     * Adding the new row to the main layout.
     */
    m_MainLayout->addItem (m_ActivationContainer);
    m_MainLayout->setStretchFactor (m_ActivationContainer, 0);
}

void 
BatteryWidget::addSliderContainer ()
{
    Q_ASSERT (m_MainLayout);

    m_SliderContainer = new SliderContainer ();
    showSlider (m_PSMAutoCombo->currentIndex () == PSMAutoOn);
}

void 
BatteryWidget::showSlider (
        bool   show)
{
    if (!m_SliderContainer)
        return;

    if (show) {
        m_SliderContainer->updateSlider (m_logic->PSMThresholdValue ());
        
        SYS_DEBUG ("SliderContainerPosition = %d", SliderContainerPosition);
        m_MainLayout->insertItem (SliderContainerPosition, m_SliderContainer);
        m_MainLayout->setStretchFactor (m_SliderContainer, 0);

        m_SliderContainer->show();
    } else {
        m_SliderContainer->hide();
        m_MainLayout->removeItem (m_SliderContainer);
    }
}

void 
BatteryWidget::addBatteryConditionWidget ()
{
    return;
    MSeparator            *spacer;
    MWidget               *pushUpWidget;
    MContainer            *container;
    QGraphicsLinearLayout *containerLayout =
        new QGraphicsLinearLayout (Qt::Vertical);
    containerLayout->setContentsMargins (0,0,0,0);
    containerLayout->setSpacing (0);

    /*
     * Creating a container and a layout.
     */
    container = new MContainer (this);
    container->setStyleName ("CommonTextFrameInverted");
    container->setHeaderVisible (false);
    container->setContentsMargins (0,0,0,0);
    container->centralWidget()->setLayout (containerLayout);

    QGraphicsLinearLayout *layout;
    layout = new QGraphicsLinearLayout (Qt::Vertical);
    layout->setContentsMargins (0,0,0,0);
    layout->setSpacing (0);

    containerLayout->addItem (layout);

    //% "Battery condition"
    MLabel *keyLabel = new MLabel (qtTrId ("qtn_ener_battery_condition"));
    keyLabel->setStyleName ("CommonTitleInverted");
    keyLabel->setAlignment (Qt::AlignLeft | Qt::AlignVCenter);
    layout->addItem (keyLabel);

    MLabel *valueLabel = new MLabel;
    valueLabel->setStyleName ("CommonSubTitleInverted");
    valueLabel->setAlignment (Qt::AlignLeft | Qt::AlignVCenter);
    layout->addItem (valueLabel);

    switch (m_logic->getCondition ())
    {
        case BatteryBusinessLogic::BGood:
        case BatteryBusinessLogic::BUnknown:
            //% "Good"
            valueLabel->setText (qtTrId ("qtn_ener_condition_good"));
            break;
        case BatteryBusinessLogic::BPoor:
            //% "Poor"
            valueLabel->setText (qtTrId ("qtn_ener_condition_poor"));
            break;
    }

    if (m_logic->getCondition () == BatteryBusinessLogic::BPoor)
    {
        QGraphicsLinearLayout *poorLayout =
            new QGraphicsLinearLayout (Qt::Horizontal);
        poorLayout->setContentsMargins (0,0,0,0);
        poorLayout->setSpacing (0);

        containerLayout->addItem (poorLayout);

        MImageWidget *poorIcon = new MImageWidget;
        poorIcon->setStyleName ("CommonSubTitleIcon");
        poorIcon->setImage ("icon-m-energy-management-insufficient-power");
        poorLayout->addItem (poorIcon);

        MLabel *poorInfo = new MLabel;
        //% "The battery capacity has decreased. You can "
        //  "improve battery life by purchasing a new battery."
        poorInfo->setText (qtTrId ("qtn_ener_condition_poor_info"));
        poorInfo->setAlignment (Qt::AlignLeft);
        poorInfo->setStyleName ("CommonSubTextInverted");
        poorInfo->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Expanding);
        poorInfo->setWordWrap (true);
        poorLayout->addItem (poorInfo);
    }

    spacer = new MSeparator;
    spacer->setStyleName ("CommonSpacer");
    containerLayout->addItem (spacer);
#if 0
    /*
     * A spacer to push up the two labels: NB#241743 (or three...)
     */
    pushUpWidget = new MWidget;
    containerLayout->addItem (pushUpWidget);
#endif
    
    /*
     * Adding the whole row to the main container.
     */
    m_MainLayout->addItem (container);
    m_MainLayout->setStretchFactor (container, 0);
}


MSeparator *
BatteryWidget::addSpacer (
        const QString &styleName, 
        int            index)
{
    MSeparator   *spacer;

    Q_ASSERT (m_MainLayout);

    spacer = new MSeparator;
    spacer->setStyleName (styleName);
    m_MainLayout->insertItem (index, spacer);

    return spacer;
}

/*!
 * This slot is called when the psm auto combo is activated.
 */
void
BatteryWidget::PSMAutoActivated (
        int PSMAutoMode)
{
    if (m_UILocked) {
        SYS_WARNING ("The UI is locked.");
    } else {
        m_logic->setPSMOption ((BatteryBusinessLogic::PowerSaveOpt)PSMAutoMode);
    }
}


void BatteryWidget::remainingBatteryCapacityReceived(const   int pct)
{
    SYS_DEBUG ("percentage = %d", pct);
    formProperBateryInfo (pct);
}

void
BatteryWidget::formProperBateryInfo (unsigned int pct)
{
    if (!m_RemainingContainer)
        return;

    /*
     * First hide the second label...
     */
    m_RemainingContainer->updateRemainingChargingTime (-1);

    if (!(m_logic->isCharging())) {
        if (!m_logic->PSMValue()) {
            m_RemainingContainer->updateCapacity (pct);
        } else {
            //% "Power save mode"
            m_RemainingContainer->setText (qtTrId ("qtn_ener_power_save_mode"));
        }
    } else {
            m_RemainingContainer->setText(qtTrId ("qtn_ener_charging"));
            m_RemainingContainer->updateRemainingChargingTime (
                    m_logic->remainingChargingTime());
    }
}

void 
BatteryWidget::PSMValueReceived (
        bool PSMEnabled)
{
    Q_UNUSED (PSMEnabled);
    m_UILocked = true;

    if (m_MainLayout && m_ActivationContainer)
    {
        formProperBateryInfo (m_logic->getBateryLevel ());
    }

    m_UILocked = false;
}

void BatteryWidget::updateUIonPowerSaveModeChange (PowerSaveOpt powerSaveopt)
{
    if (powerSaveopt == PSMAutoAutomatic)
    {
        showSlider (true);
    } else {
        showSlider (false);
    }
}

void
BatteryWidget::retranslateUi ()
{
    if (m_TitleLabel)
        //% "Battery"
        m_TitleLabel->setText (qtTrId("qtn_ener_battery"));

    // FIXME: Why do we need this?
    m_logic->remainingCapacityRequired();
}

void BatteryWidget::charging(int animation_rate)
{
    Q_UNUSED (animation_rate);
    SYS_DEBUG("Charging rate: %d", animation_rate);

    formProperBateryInfo (m_logic->getBateryLevel ());
}

void BatteryWidget::chargeComplete()
{
    //% "Charging complete"
    m_RemainingContainer->setText(qtTrId ("qtn_ener_charcomp"));
    m_RemainingContainer->updateRemainingChargingTime (-1);
}
