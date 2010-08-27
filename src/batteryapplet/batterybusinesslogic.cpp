/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */
/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary (-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of battery-applet.
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
#include "batterybusinesslogic.h"
#include <MGConfItem>
#include <QVariant>
#include <QString>

#define DEBUG
#define WARNING
#include "../debug.h"

static const QString psm_auto_key =
    "/system/osso/dsm/energymanagement/enable_power_saving";
static const QString psm_values_key =
    "/system/osso/dsm/energymanagement/possible_psm_thresholds";

static const int animation_rate_charging_usb  = 500;
static const int animation_rate_charging_wall = 250;


BatteryBusinessLogic::BatteryBusinessLogic (
        QObject *parent)
    : QObject (parent),
    m_initialized (false),
    m_ChargingRate (-1),
    m_Charging (false)
{
    m_battery    = new QmBattery (this);
    m_devicemode = new QmDeviceMode (this);
}

BatteryBusinessLogic::~BatteryBusinessLogic ()
{
}

/*
 * This function does the initialization and signal
 * connection to QmBattery and QmDeviceMode, and emits
 * all the signals with the current values...
 *
 * FIXME: This function should be called realize(9 or something...
 */
void
BatteryBusinessLogic::requestValues ()
{
    if (m_initialized)
        return;

    m_initialized = true;

    SYS_DEBUG ("Connecting to the signals of the QmBattery class");
    connect (
        m_battery, SIGNAL(chargerEvent(Maemo::QmBattery::ChargerType)),
        this, SLOT(batteryChargerEvent(Maemo::QmBattery::ChargerType)));
    connect (
        m_battery, SIGNAL(chargingStateChanged(Maemo::QmBattery::ChargingState)),
        this, SLOT(chargingStateChanged(Maemo::QmBattery::ChargingState)));
    connect (
        m_battery, SIGNAL(batteryStateChanged(Maemo::QmBattery::BatteryState)),
        this, SLOT (batteryStateChanged(Maemo::QmBattery::BatteryState)));

    // This will emit the batteryCharging signal,
    // and the remainingTimeValuesChanged signal
    chargingStateChanged (m_battery->getChargingState ());

    /*
     * We have two signals showing that the battery energy level has been
     * changed. We listen both of these signals.
     */
    SYS_DEBUG ("Connecting to the signals of the QmBattery class");
    connect (m_battery, SIGNAL (batteryRemainingCapacityChanged (int, int)),
            this, SLOT (batteryRemCapacityChanged (int, int)));


    // batteryBarValueReceived also emitted by chargingStateChanged ^^^
    // FIXME: Why?
    connect (m_devicemode,
             SIGNAL (devicePSMStateChanged (Maemo::QmDeviceMode::PSMState)),
             this, SLOT (PSMStateChanged (Maemo::QmDeviceMode::PSMState)));

    SYS_DEBUG ("Emitting PSMValueReceived(%s)",
            SYS_BOOL(m_devicemode->getPSMState () == QmDeviceMode::PSMStateOn));
    emit PSMValueReceived (m_devicemode->getPSMState () ==
                           QmDeviceMode::PSMStateOn);
}

void
BatteryBusinessLogic::setPSMThresholdValue (
        int percentage)
{
    bool ret;
    ret = m_devicemode->setPSMBatteryMode (percentage);

    if (!ret) {
        SYS_WARNING (" failed to set (precentage = %d)", percentage);
    }
}

int
BatteryBusinessLogic::PSMThresholdValue ()
{
    return m_devicemode->getPSMBatteryMode ();
}

QStringList
BatteryBusinessLogic::PSMThresholdValues ()
{
    MGConfItem  possible_values (psm_values_key);
    QStringList retval;

    retval = possible_values.value ().toStringList ();
    if (retval.size() == 0) {
        SYS_WARNING ("The GConf key %s is not set. Falling back to default.",
                SYS_STR(psm_values_key));
        retval << "10" << "20" << "30" << "40" << "50";
    }

    return retval;
}

void
BatteryBusinessLogic::setPSMValue (
        bool enabled)
{
    bool ret;
    ret = m_devicemode->setPSMState (
        enabled ?
        QmDeviceMode::PSMStateOn :
        QmDeviceMode::PSMStateOff);

    if (ret) {
        // Change succeed, we don't need to wait for QmSystem reply, we can emit
        // the PSMValueChanged asap to update the UI.
        SYS_DEBUG ("Emitting PSMValueReceived(%s)", SYS_BOOL(enabled));
        emit PSMValueReceived (enabled);
    } else {
        SYS_WARNING ("Failed to set PSM mode to %s",
                enabled ? 
                "QmDeviceMode::PSMStateOn" : "QmDeviceMode::PSMStateOff");
    }
}

/*!
 * Sets the PSMAutoValue, a boolean that sets if the device should go into power
 * save mode automatically. This function only changes a value in the GConf
 * database.
 */
void
BatteryBusinessLogic::setPSMAutoValue (
        bool toggle)
{
    MGConfItem PSMAutoKey (psm_auto_key);
    PSMAutoKey.set (toggle);
}

/*!
 * Reads the PSMAuto value, a boolean that sets if the device should go into
 * power save mode automatically. This method is only reads the GConf database.
 */
bool
BatteryBusinessLogic::PSMAutoValue ()
{
    MGConfItem PSMAutoKey (psm_auto_key);
    return PSMAutoKey.value ().toBool ();
}

/*!
 * This function will amit a signal with the remaining times for the battery as
 * a list of strings. When the battery is not charging the return value contains
 * numbers as strings, when the battery is on charger the return string is a UI
 * string representing the charging state.
 *
 * FIXME: Maybe this class should not contain localization, it should return
 * only the logical values?
 */
//void
//BatteryBusinessLogic::remainingTimeValuesRequired ()
//{
//    QmBattery::RemainingTimeMode mode;
//    QStringList                  values;
//    int remainingBatteryCapacityPercentage;

//    /*
//     * If the charger QmSystem reports charging there might be no actual
//     * charging since the battery might be full. We still show charging however,
//     * otherwise we would show that the phone has a limited time to stay on and
//     * we don't want to say that while we are on charger...
//     */
//    if (m_Charging) {
//        //% "Charging"
//        values << qtTrId ("qtn_ener_charging")
//               << qtTrId ("qtn_ener_charging");
//        goto finalize;
//    }

//    switch (m_devicemode->getPSMState ()) {
//        case QmDeviceMode::PSMStateOn:
//            mode = QmBattery::PowersaveMode;
//            break;

//        case QmDeviceMode::PSMStateOff:
//        default:
//             mode = QmBattery::NormalMode;
//             break;
//    }
    
//    values << QString ("%1").arg (m_battery->getRemainingTalkTime (mode) / 60)
//           << QString ("%1").arg (m_battery->getRemainingIdleTime (mode) / 60);

//    remainingBatteryCapacityPercentage =
//            m_battery->getRemainingCapacityPct();

//finalize:
//    SYS_DEBUG ("Emitting remainingTimeValuesChanged()");
//    emit remainingTimeValuesChanged (values);
//    emit remainingBatteryCapacityChanged(remainingBatteryCapacityPercentage);
//}

void
BatteryBusinessLogic::remainingCapacityRequired()
{
//    SYS_DEBUG("BATTERY REMAINING CAPACITY: %d", m_battery->getRemainingCapacityPct());
    emit remainingBatteryCapacityChanged(
            m_battery->getRemainingCapacityPct());
}

/*!
 * We have three functions her that notify us about the changes in the charging
 * state. We use all of the three slots the same way, but we keep them
 * separately so we can actually see why do we need to recalculate the charging
 * info.
 */
void
BatteryBusinessLogic::batteryChargerEvent (
        Maemo::QmBattery::ChargerType type)
{
    Q_UNUSED (type);

    SYS_DEBUG ("");
    recalculateChargingInfo ();
}

void
BatteryBusinessLogic::chargingStateChanged (
        Maemo::QmBattery::ChargingState state)
{
    Q_UNUSED (state);
    
    SYS_DEBUG ("");
    recalculateChargingInfo ();
}

void 
BatteryBusinessLogic::batteryStateChanged (
        Maemo::QmBattery::BatteryState batteryState)
{
    Q_UNUSED (batteryState);
    
    SYS_DEBUG ("");
    recalculateChargingInfo ();
}

/*!
 * This slot will be called when the device power save mode is changed. The
 * method will send the PSMValueReceived() signal.
 */
void
BatteryBusinessLogic::PSMStateChanged (
        Maemo::QmDeviceMode::PSMState state)
{
    bool enabled = state == QmDeviceMode::PSMStateOn;
    
    SYS_DEBUG ("Emitting PSMValueReceived (%s)", SYS_BOOL(enabled));
    emit PSMValueReceived (enabled);
}

void
BatteryBusinessLogic::batteryRemCapacityChanged (
		int percentage, 
		int bars)
{
    Q_UNUSED (bars);
    SYS_DEBUG ("**** percentage = %d, bars = %d [max = %d]",
               percentage, bars, m_battery->getMaxBars ());

    // XXX: FIXME: maybe we can drop batteryBarValue and use 'bars' parameter..
    SYS_DEBUG ("Emitting batteryBarValueReceived(%d)",
            batteryBarValue (percentage));
    emit batteryBarValueReceived (batteryBarValue (percentage));
    emit remainingBatteryCapacityChanged(percentage);
}

/*!
 * \param percentage The energy level percentage or -1 to ask the backend.
 *
 * Returns the bar value (the value that used as an icon index representing for
 * the battery energy level) for the given percentage level. If the argument is
 * -1 this method will ask the QmBattery for the current energy level.
 *
 */
int
BatteryBusinessLogic::batteryBarValue (
        int percentage)
{
    int index;

    if (percentage == -1) {
        percentage = m_battery->getRemainingCapacityPct ();
    }

    // in case of overflow (eg. in sbox) when we get value that greater than 100
    // percent we use 10 percent intentionaly
    if (percentage < 0)
        percentage = 0;
    else if (percentage > 100)
        percentage = 10;

    if (percentage >= 84)
        index = 9;
    else if (percentage < 84 && percentage >= 73)
        index = 8;
    else if (percentage < 73 && percentage >= 62)
        index = 7;
    else if (percentage < 62 && percentage >= 51)
        index = 6;
    else if (percentage < 51 && percentage >= 39)
        index = 5;
    else if (percentage < 39 && percentage >= 28)
        index = 4;
    else if (percentage < 28 && percentage >= 17)
        index = 3;
    else if (percentage < 17 && percentage >= 5)
        index = 2;
    else if (percentage < 5 && percentage > 1)
        index = 1;
    else // if (percentage == 0)
        index = 0;

    return index;
}

/*!
 * This function is called whenever some change is detected around the battery
 * and its charger. This method will try to find out if the battery is actually
 * charging and if it is the method will calculate the charging animation speed.
 *
 * What we managed to find aboput the charging is:
 * 1) The battery is not charging (despite any other info returned) if the
 *    battery is full. See NB#172929 for further details.
 */
void
BatteryBusinessLogic::recalculateChargingInfo ()
{
    QmBattery::ChargingState chargingState;
    QmBattery::BatteryState  batteryState;
    QmBattery::ChargerType   chargerType;
    bool                     charging;
    bool                     couldBeCharging;
    int                      chargingRate;
   
    chargerType = m_battery->getChargerType ();
    chargingState = m_battery->getChargingState ();
    batteryState = m_battery->getBatteryState ();

    /*
     * Carefully calculating the charging rate, the animation rate of the
     * charging indicator.
     */
    couldBeCharging =
        chargingState == QmBattery::StateCharging &&
        // This is actually not necessary, but we even check it in the unit
        // test. Just to be sure.
        chargerType != QmBattery::None;

    charging = 
        batteryState != QmBattery::StateFull &&
        chargerType != QmBattery::None &&
        chargingState != QmBattery::StateNotCharging &&
        chargingState != QmBattery::StateChargingFailed;

    chargingRate = 0;
    if (charging) 
        chargingRate = chargerType == QmBattery::Wall ?
            animation_rate_charging_wall : animation_rate_charging_usb;

    SYS_DEBUG ("*** charging       = %s", SYS_BOOL(charging));
    SYS_DEBUG ("*** m_Charging     = %s", SYS_BOOL(m_Charging));
    SYS_DEBUG ("*** chargingRate   = %d", chargingRate);
    SYS_DEBUG ("*** m_ChargingRate = %d", m_ChargingRate);

    if (chargingRate == m_ChargingRate &&
            couldBeCharging == m_Charging) 
        return;

    /*
     * If the charging rate has been changed we need to notify the ui with a
     * signal.
     */
    m_Charging = couldBeCharging;
    SYS_DEBUG ("*** chargingRate %d -> %d", m_ChargingRate, chargingRate);
    m_ChargingRate = chargingRate;       
    SYS_DEBUG ("Emitting batteryCharging(%d)", m_ChargingRate);
    emit batteryCharging (m_ChargingRate);

    /*
     * Then we need to notify everyone about the bar value. 
     * FIXME: Why exactly do we need that?
     */
    SYS_DEBUG ("Emitting batteryBarValueReceived(%d)", batteryBarValue (-1));
    emit batteryBarValueReceived (batteryBarValue (-1));

    if(batteryState == QmBattery::StateFull)
    {
        emit batteryFull();
        return;
    }

    /*
     * And the remaining battery capacity has to be recalculated.
     */
    remainingCapacityRequired();
}

bool BatteryBusinessLogic::isCharging()
{
    return m_Charging;
}
