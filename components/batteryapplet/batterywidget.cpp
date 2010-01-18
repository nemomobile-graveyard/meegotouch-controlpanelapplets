#include "batterywidget.h"
#include "batteryimage.h"
#include "batterydbusinterface.h"
#include "dcpbattery.h"
#include "slidercontainer.h"
#include "timecontainer.h"

#include <QGraphicsLinearLayout>
#include <QDebug>

#include <DuiButton>
#include <DuiContainer>
#include <DuiGridLayoutPolicy>
#include <DuiLayout>
#include <DuiLinearLayoutPolicy>

/*

TODO list:

1) what is the correct interval for updating the battery image when charging? Is there a difference between
   USB and normal charging?

*/

BatteryWidget::BatteryWidget(QGraphicsWidget *parent) :
        DcpWidget(parent),
        batteryIf(NULL),
        batteryImage(NULL),
        PSMButton(NULL),
        sliderContainer(NULL),
        standByTimeContainer(NULL),
        talkTimeContainer(NULL)
{
    setReferer(DcpBattery::None);
    initWidget();
}

BatteryWidget::~BatteryWidget()
{
}

bool BatteryWidget::back()
{
    return true; // back is handled by main window by default
}

void BatteryWidget::initWidget()
{
    // proxy for dbus interface on remote object
    batteryIf = new BatteryDBusInterface();

    // battery image
    batteryImage = new BatteryImage();

    // talkTimeContainer
    //% "Estimated talk time:"
    talkTimeContainer = new TimeContainer(qtTrId ("qtn_ener_tt"),
                                          batteryImage);

    // standByTimeContainer
    //% "Estimated stand-by time:"
    standByTimeContainer = new TimeContainer(qtTrId ("qtn_ener_st"),
                                             new DuiImage("qgn_ener_standby"));

    // PSMButton
    PSMButton = new DuiButton();
    connect(PSMButton, SIGNAL(released()), this, SLOT(PSMButtonReleased()));

    // sliderContainer
    sliderContainer = new SliderContainer();
    connect (sliderContainer, SIGNAL(PSMAutoToggled(bool)),
             batteryIf, SLOT(setPSMAutoValue(bool)));
    connect (sliderContainer, SIGNAL(PSMThresholdValueChanged(QString)),
             batteryIf, SLOT(setPSMThresholdValue(QString)));

    // mainContainer
    DuiLayout *orientationLayout = new DuiLayout();

    DuiGridLayoutPolicy *landscapeLayoutPolicy = new DuiGridLayoutPolicy(orientationLayout);
    landscapeLayoutPolicy->addItemAtPosition(talkTimeContainer, 0, 0);
    landscapeLayoutPolicy->addItemAtPosition(standByTimeContainer, 0, 1);
    landscapeLayoutPolicy->addItemAtPosition(PSMButton, 1, 0, 1, 2);
    landscapeLayoutPolicy->addItemAtPosition(sliderContainer, 2, 0, 1, 2);
    landscapeLayoutPolicy->setSpacing(10);
    orientationLayout->setLandscapePolicy(landscapeLayoutPolicy);

    DuiLinearLayoutPolicy *portraitLayoutPolicy = new DuiLinearLayoutPolicy(orientationLayout, Qt::Vertical);
    portraitLayoutPolicy->addItem(talkTimeContainer, Qt::AlignLeft);
    portraitLayoutPolicy->addItem(standByTimeContainer, Qt::AlignLeft);
    portraitLayoutPolicy->addItem(PSMButton, Qt::AlignCenter);
    portraitLayoutPolicy->addItem(sliderContainer, Qt::AlignLeft);
    portraitLayoutPolicy->setSpacing(10);
    orientationLayout->setPortraitPolicy(portraitLayoutPolicy);

    DuiContainer *mainContainer = new DuiContainer();
    mainContainer->setHeaderVisible(false);
    mainContainer->centralWidget()->setLayout(orientationLayout);

    // connect the value receive signals
    connect (batteryIf, SIGNAL(remainingTimeValuesReceived(QStringList)),
             this, SLOT(remainingTimeValuesReceived(QStringList)));
    connect (batteryIf, SIGNAL(batteryCharging(int)),
             batteryImage, SLOT(startCharging(int)));
    connect (batteryIf, SIGNAL(batteryNotCharging()),
             batteryImage, SLOT(stopCharging()));
    connect (batteryIf, SIGNAL(batteryNotCharging()),
             batteryIf, SLOT(batteryBarValueRequired()));
    connect (batteryIf, SIGNAL(batteryBarValueReceived(int)),
             batteryImage, SLOT(updateBatteryLevel(int)));
    connect (batteryIf, SIGNAL(PSMValueReceived(bool)),
             this, SLOT(updatePSMButton(bool)));
    connect (batteryIf, SIGNAL(PSMAutoValueReceived(bool)),
             sliderContainer, SLOT(initPSMAutoButton(bool)));
    connect (batteryIf, SIGNAL(PSMAutoDisabled()),
             sliderContainer, SLOT(PSMAutoDisabled()));
    connect (batteryIf, SIGNAL(PSMThresholdValuesReceived(QStringList)),
             sliderContainer, SLOT(initSlider(QStringList)));
    connect (batteryIf, SIGNAL(PSMThresholdValuesReceived(QStringList)),
             batteryIf, SLOT(PSMThresholdValueRequired()));
    connect (batteryIf, SIGNAL(PSMThresholdValueReceived(QString)),
             sliderContainer, SLOT(updateSlider(QString)));

    // send value requests over dbus
    batteryIf->remainingTimeValuesRequired();
    batteryIf->batteryBarValueRequired();
    batteryIf->batteryChargingStateRequired();
    batteryIf->PSMValueRequired();
    batteryIf->PSMAutoValueRequired();
    batteryIf->PSMThresholdValuesRequired();

    // mainLayout
    QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addItem(mainContainer);
    setLayout(mainLayout);

}

void BatteryWidget::PSMButtonReleased()
{
    batteryIf->setPSMValue (PSMButtonToggle);
}

void BatteryWidget::updatePSMButton(bool toggle)
{
    PSMButtonToggle = toggle;

    if (toggle) {
        //% "Deactivate power save now"
        PSMButton->setText (qtTrId ("qtn_ener_dps"));
    } else {
        //% "Activate power save now"
        PSMButton->setText (qtTrId ("qtn_ener_aps"));
    }
}

void BatteryWidget::remainingTimeValuesReceived(const QStringList &timeValues)
{
    qDebug() << "BatteryWidget::remainingTimeValuesReceived(" << timeValues.at(0) << ", " << timeValues.at(1) << ")";
    talkTimeContainer->updateTimeLabel(timeValues.at(0));
    standByTimeContainer->updateTimeLabel(timeValues.at(1));
}

void BatteryWidget::retranslateUi ()
{
    // This call will reload the translated text on PSButton
    updatePSMButton (PSMButtonToggle);

    // This call will retranslate the label (infoText)
    sliderContainer->retranslate ();

    talkTimeContainer->setText(qtTrId ("qtn_ener_tt"));
    standByTimeContainer->setText (qtTrId ("qtn_ener_st"));

    // This call will reload timelabels on timercontainers
    batteryIf->remainingTimeValuesRequired();

}