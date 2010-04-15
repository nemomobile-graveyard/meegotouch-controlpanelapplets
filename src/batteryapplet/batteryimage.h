/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */
#ifndef BATTERYIMAGE_H
#define BATTERYIMAGE_H

#include <MImageWidget>

class QTimer;

class BatteryImage : public MImageWidget
{
    Q_OBJECT

public:
    BatteryImage (QGraphicsItem *parent = 0);
    ~BatteryImage ();

public slots:
    void updateBatteryLevel (int level);
    void startCharging (int rate);
    void stopCharging ();

private slots:
    void updateImage (bool charging = true);

private:
    QTimer                 *m_timer;
    int                     m_batteryLevel;
    QList<const QPixmap *>  m_Images;
    QList<const QPixmap *>  m_ChargingImages;

    void loadImages (bool charging);
};

#endif // BATTERYIMAGE_H
