/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */
#include "batterywidget.h"
#include "batteryimage.h"
#include "batterybusinesslogic.h"
#include "dcpbattery.h"
#include "slidercontainer.h"
#include "percentagecontainer.h"

#include <QGraphicsLinearLayout>
#include <QTimer>

#include <MButton>
#include <MContainer>

#undef DEBUG
#define WARNING
#include "../debug.h"

BatteryWidget::BatteryWidget (QGraphicsWidget *parent) :
        DcpWidget (parent),
        m_logic (0),
        m_UILocked (false),
        batteryImage (0),
        PSMButton (0),
        m_MainLayout (0),
        sliderContainer (0),
        remainingCapacityContainer (0)
{
    SYS_DEBUG ("Starting in %p", this);
    /*
     * One can assume, that when the applet is started the power save mode is
     * not active.
     */
    m_PSMButtonToggle = false;

    initWidget ();
}

BatteryWidget::~BatteryWidget ()
{
    SYS_DEBUG ("Destroying %p", this);

    if (m_logic)
    {
        delete m_logic;
        m_logic = 0;
    }
}

bool BatteryWidget::back ()
{
    return true; // back is handled by main window by default
}

void BatteryWidget::initWidget ()
{
    SYS_DEBUG ("Start");
    // instantiate the batterybusinesslogic
    m_logic = new BatteryBusinessLogic;

    // battery image
    batteryImage = new BatteryImage;

    // batteryRemainingCapacityPercentage
    //% "Battery level \%L1\%"
    remainingCapacityContainer = new PercentageContainer (
            qtTrId ("qtn_ener_battery_level"), batteryImage);

    connect (m_logic, SIGNAL(batteryCharging (int)),
             this, SLOT(charging(int)));

    m_logic->remainingCapacityRequired();

    /*
     * PSMButton is used to immediately turn the power save mode on/off.
     */
    PSMButton = new MButton;
    PSMButton->setObjectName ("PSMButton");
    updatePSMButton ();

    connect (PSMButton, SIGNAL (released ()),
             this, SLOT (PSMButtonReleased ()));

    /*
     * sliderContainer
     */
    sliderContainer = new SliderContainer;

    /*
     * SliderContainer signals and slots,
     * and initialization
     */
    sliderContainer->initPSMAutoButton (m_logic->PSMAutoValue ());
    sliderContainer->initSlider (m_logic->PSMThresholdValues ());
    sliderContainer->updateSlider (m_logic->PSMThresholdValue ());

    connect (sliderContainer, SIGNAL (PSMAutoToggled (bool)),
             this, SLOT (PSMAutoToggled (bool)));
    connect (sliderContainer, SIGNAL (PSMThresholdValueChanged (int)),
             m_logic, SLOT (setPSMThresholdValue (int)),
             Qt::DirectConnection);

    // mainContainer
    m_MainLayout = new QGraphicsLinearLayout (Qt::Vertical);

    m_MainLayout->addItem (remainingCapacityContainer);
    m_MainLayout->addItem (sliderContainer);
    m_MainLayout->addItem (PSMButton);
    m_MainLayout->setAlignment (PSMButton, Qt::AlignCenter);

    MContainer *mainContainer = new MContainer;
    mainContainer->setHeaderVisible (false);
    mainContainer->centralWidget ()->setLayout (m_MainLayout);

    // connect the value receive signals
    connect (m_logic, SIGNAL(remainingBatteryCapacityChanged(int)),
             this, SLOT(remainingBatteryCapacityReceived(int)));

    /*
     * Connect the batteryImage slots.
     */
    connect (m_logic, SIGNAL (batteryCharging (int)),
             batteryImage, SLOT (startCharging (int)));
    connect (m_logic, SIGNAL (batteryBarValueReceived (int)),
             batteryImage, SLOT (updateBatteryLevel (int)));
    connect (m_logic, SIGNAL (PSMValueReceived (bool)),
             batteryImage, SLOT (setPSMValue (bool)));

    connect (m_logic, SIGNAL (PSMValueReceived (bool)),
             this, SLOT (PSMValueReceived (bool)));

    connect (m_logic, SIGNAL(batteryFull()),
             batteryImage, SLOT(chargeComplete()));
    connect (m_logic, SIGNAL(batteryFull()),
             this, SLOT(chargeComplete()));
    // mainLayout
    QGraphicsLinearLayout *mainLayout =
        new QGraphicsLinearLayout (Qt::Vertical);

    mainLayout->setContentsMargins (0, 0, 0, 0);
    mainLayout->addItem (mainContainer);
    mainLayout->addStretch ();
    setLayout (mainLayout);

    // Initialize the values from the business logic
    m_logic->requestValues ();

    SYS_DEBUG ("End");
}

/*!
 * This function is called when the user clicked on the 'power save mode' button
 * that activates and disactivates the power saving mode. The function will call
 * the battery interface, but the UI will be changed only when the power save
 * mode really changed...
 */
void 
BatteryWidget::PSMButtonReleased ()
{
    bool newPSMValue = !m_PSMButtonToggle;
    SYS_DEBUG ("Setting PSMvalue to %s", SYS_BOOL (newPSMValue));

    /*
     * To 'deactivate' the power-save move, we need to turn
     * off the automatic power-saving...
     */
    if (newPSMValue == false)
    {
        m_logic->setPSMAutoValue (false);
        sliderContainer->initPSMAutoButton (false);
    }

    // UI will change only in PSMValueReceived slot...
    m_logic->setPSMValue (newPSMValue);
}

/*!
 * This slot is called when the psm auto switch is toggled.
 */
void
BatteryWidget::PSMAutoToggled (
        bool PSMAutoEnabled)
{
    SYS_DEBUG ("*** PSMAutoEnabled = %s", SYS_BOOL(PSMAutoEnabled));

    if (m_UILocked) {
        SYS_WARNING ("The UI is locked.");
    } else {
        m_logic->setPSMAutoValue (PSMAutoEnabled);

        if (PSMAutoEnabled)
        {
            /*
             * QmSystem returns 0 when PSMAuto is disabled,
             * so when we're enabling it, we've to re-query
             * the proper value
             */
            sliderContainer->updateSlider (m_logic->PSMThresholdValue ());
        }
    }
}

void 
BatteryWidget::updatePSMButton ()
{
    if (m_PSMButtonToggle) {
        //% "Deactivate power save now"
        PSMButton->setText (qtTrId ("qtn_ener_dps"));
    } else {
        //% "Activate power save now"
        PSMButton->setText (qtTrId ("qtn_ener_aps"));
    }
}

void BatteryWidget::remainingBatteryCapacityReceived(const int pct)
{
    SYS_DEBUG ("percentage = %d", pct);
    if(!(m_logic->isCharging()))
    {
        if(!m_PSMButtonToggle)
        {
            remainingCapacityContainer->updateCapacity (pct);
        }
        else
        {
            //% "Power save mode"
            remainingCapacityContainer->setText (qtTrId ("qtn_ener_power_save_mode"));
        }
    }
    else
    {
        if(!m_PSMButtonToggle)
        {
            //% "Charging"
            remainingCapacityContainer->setText(qtTrId ("qtn_ener_charging"));
        }
        else
        {
            //% "Power save mode"
            remainingCapacityContainer->setText (qtTrId ("qtn_ener_power_save_mode"));
        }
    }
}

void 
BatteryWidget::PSMValueReceived (
        bool PSMEnabled)
{
    SYS_DEBUG ("*** PSMEnabled = %s", SYS_BOOL (PSMEnabled));

    if (m_PSMButtonToggle == PSMEnabled) {
        SYS_DEBUG ("toggle already set");
        return;
    }
    m_PSMButtonToggle = PSMEnabled;

    updatePSMButton ();
    m_UILocked = true;

    if (!PSMEnabled)
    {
        m_MainLayout->insertItem (1, sliderContainer);
        sliderContainer->setVisible (true);
        m_logic->remainingCapacityRequired();
    }
    else
    {
        sliderContainer->setVisible (false);

        m_MainLayout->removeItem (sliderContainer);
        //% "Power save mode"
        remainingCapacityContainer->setText (qtTrId ("qtn_ener_power_save_mode"));
    }
    m_MainLayout->invalidate ();

    m_UILocked = false;
}

void
BatteryWidget::retranslateUi ()
{
    // This call will reload the translated text on PSButton
    updatePSMButton ();

    // This call will retranslate the label (infoText)
    sliderContainer->retranslate ();

    m_logic->remainingCapacityRequired();
}

void BatteryWidget::charging(int animation_rate)
{
    SYS_DEBUG("Charging rate: %d", animation_rate);
    if(animation_rate > 0)
    {
        //% "Charging"
        remainingCapacityContainer->setText(qtTrId ("qtn_ener_charging"));
    }
}

void BatteryWidget::chargeComplete()
{
    //% "Charging complete"
    remainingCapacityContainer->setText(qtTrId ("qtn_ener_charcomp"));
}
