/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */
#ifndef BATTERYWIDGET_H
#define BATTERYWIDGET_H

#include "dcpwidget.h"

class MButton;
class BatteryDBusInterface;
class BatteryImage;
class SliderContainer;
class TimeContainer;

class BatteryWidget : public DcpWidget
{
    Q_OBJECT

public:
    BatteryWidget (QGraphicsWidget *parent = 0);
    ~BatteryWidget ();
    bool back();

protected:
    void initWidget();

private slots:
    void PSMButtonReleased();
    void remainingTimeValuesReceived(const QStringList &timeValues);
    void PSMValueReceived (bool PSMEnabled);
    void PSMAutoToggled (bool PSMAutoEnabled);
private:
    void updatePSMButton();
    
private:
    BatteryDBusInterface    *batteryIf;
    bool                     m_UILocked;
    BatteryImage            *batteryImage;
    MButton                 *PSMButton;
    // true means we are believed to be in power save mode
    bool                     m_PSMButtonToggle;
    SliderContainer         *sliderContainer;
    TimeContainer           *standByTimeContainer;
    TimeContainer           *talkTimeContainer;

    void    retranslateUi ();

};
#endif // BATTERYWIDGET_H
