/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef BATTERYWIDGET_H
#define BATTERYWIDGET_H

#include "dcpwidget.h"

class MButton;
class MLabel;
class MLayout;
class MLinearLayoutPolicy;
class MContainer;
class BatteryBusinessLogic;
class BatteryImage;
class SliderContainer;
class PercentageContainer;

class BatteryWidget : public DcpWidget
{
    Q_OBJECT

public:
    BatteryWidget (QGraphicsWidget *parent = 0);
    ~BatteryWidget ();
    bool back ();

protected:
    void initWidget();

private slots:
    void PSMButtonReleased();
    void remainingBatteryCapacityReceived (const int value);
    void PSMValueReceived (bool PSMEnabled);
    void PSMAutoToggled (bool PSMAutoEnabled);
    void charging(int animation_rate);
    void chargeComplete ();

private:
    void addRemainingCapacityWidget ();
    void addAutoActivationWidget ();
    void addSliderContainer ();
    void addPowerSaveButton ();
    
    void updatePSMButton ();
    void retranslateUi ();

private: 
    BatteryBusinessLogic    *m_logic;
    MLinearLayoutPolicy     *m_MainLayout;
    PercentageContainer     *m_RemainingContainer;
    MContainer              *m_ActivationContainer;
    SliderContainer         *m_SliderContainer;
    MButton                 *m_PSMAutoButton;
    MButton                 *PSMButton;
    BatteryImage            *m_BatteryImage;
    bool                     m_UILocked;
    // true means we are believed to be in power save mode
    bool                     m_PSMButtonToggle;
};

#endif // BATTERYWIDGET_H
