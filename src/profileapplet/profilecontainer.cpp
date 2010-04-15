/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */

#include "profilecontainer.h"
#include "profilebuttons.h"

#include <QGraphicsLinearLayout>
#include <MButton>
#include <MSlider>
#include <MLabel>
#include <MLayout>
#include <MImageWidget>
#include <MGridLayoutPolicy>
#include <MLinearLayoutPolicy>
#include <MSeparator>
#include <QDebug>

#undef DEBUG
#include "../debug.h"

namespace ProfileApplet
{
    const int sliderMin = 0;
    const int sliderMax = 100;
}

ProfileContainer::ProfileContainer (
        int            id, 
        const QString &title, 
        int            level, 
        bool           vibra, 
        MWidget     *parent) :
    MContainer (title, parent),
    m_Slider (0),
    m_NoRingLabel (0),
    m_Img (0),
    m_Level (-2),
    m_ProfileId (id)
{
    SYS_DEBUG ("Creating container for %s", SYS_STR(title)); 

    if (0 <= level) {
        /*
         * Had to remove the 'continuous' for it ruined the theming of the
         * slider, the height become huge.
         */
        m_Slider = new MSlider (0/*, "continuous"*/);
        m_Slider->setOrientation (Qt::Horizontal);
        m_Slider->setRange (ProfileApplet::sliderMin, ProfileApplet::sliderMax);
        #if 0
        /*
         * Ok, this worth a try, but did not work. Maybe we should try it again,
         * with some other version of the libdui.
         */
        m_Slider->setMinLabelVisible (true);
        m_Slider->setMinLabelIconID ("icon-m-common-volume");
        #endif

        setLevel (level);
        connect (m_Slider, SIGNAL (valueChanged(int)), 
                this, SIGNAL (sliderValueChanged(int)));
        connect (m_Slider, SIGNAL (valueChanged(int)), 
                this, SLOT (slotSliderValueChanged(int)));
    }

    //% "Vibration"
    m_VibrationLabel = new MLabel (qtTrId ("qtn_prof_vibration"));

    m_OnOffLabel = new MLabel ();
    m_OnOffLabel->setObjectName ("OnOffLabel");

    m_VibrationSwitch = new MButton;
    m_VibrationSwitch->setCheckable (true);
    m_VibrationSwitch->setViewType (MButton::switchType);


    setVibration (vibra);
    connect (m_VibrationSwitch, SIGNAL(toggled(bool)), 
            this, SLOT(slotVibrationSwitchToggled(bool)));

    setLayout ();
}

ProfileContainer::~ProfileContainer()
{
    SYS_DEBUG ("");
}

void 
ProfileContainer::setLayout ()
{
    QGraphicsLayoutItem   *item;
    QGraphicsLinearLayout *hbox, *hbox1, *vbox;
    MLayout             *layout = new MLayout();
    MSeparator          *vSep, *hSep;
    MLinearLayoutPolicy *landscapePolicy, *portraitPolicy;
    
    SYS_DEBUG ("");

    landscapePolicy = new MLinearLayoutPolicy (layout, Qt::Horizontal);
    portraitPolicy  = new MLinearLayoutPolicy (layout, Qt::Vertical);
    #if 0
    layout->setLandscapePolicy (portraitPolicy);
    layout->setPortraitPolicy (landscapePolicy);
    #else
    layout->setLandscapePolicy (landscapePolicy);
    layout->setPortraitPolicy (portraitPolicy);
    #endif
    /*
     * FIXME: Ok, but what happens when we change the 'mute' state?
     */
    if (m_Slider) {
        m_Img = new MImageWidget ("icon-m-common-volume");
        item = m_Slider;
    } else {
        m_Img = new MImageWidget ("icon-m-common-volume-off");
        //% "No ringing"
        item = m_NoRingLabel = new MLabel (qtTrId ("qtn_prof_noring"));
    }
    m_Img->setObjectName ("speakerIcon");

    vSep = new MSeparator (this, Qt::Vertical);
    hSep = new MSeparator (this, Qt::Horizontal);

    vbox = new QGraphicsLinearLayout (Qt::Vertical);
    vbox->setSpacing (0.0);
    vbox->addItem (m_VibrationLabel);
    vbox->addItem (m_OnOffLabel);

    hbox = new QGraphicsLinearLayout (Qt::Horizontal);
    hbox->addItem (vbox);
    hbox->addItem (m_VibrationSwitch);
    hbox->setAlignment (vbox, Qt::AlignCenter);
    hbox->setAlignment (m_VibrationSwitch, Qt::AlignCenter);

    hbox1 = new QGraphicsLinearLayout (Qt::Horizontal);
    hbox1->addItem (m_Img);
    hbox1->addItem (item);
    hbox1->setAlignment (m_Img, Qt::AlignCenter);
    hbox1->setAlignment (item, Qt::AlignCenter);

    /*
     * Adding the widgets to the proper places.
     */
    landscapePolicy->addItem (hbox1);
    landscapePolicy->addItem (vSep);
    landscapePolicy->addItem (hbox);
    
    portraitPolicy->addItem (hbox1);
    portraitPolicy->addItem (hSep);
    portraitPolicy->addItem (hbox);
    
    centralWidget()->setLayout(layout);
}

void 
ProfileContainer::setLevel (
        int value)
{
    qDebug() << Q_FUNC_INFO << "for" << title() << ":" << value << 
        "(old:" << m_Level << ")";

    if (!m_Slider || value == m_Level)
        return;

    m_Level = value;
    if (ProfileApplet::sliderMin < m_Level && 
            ProfileApplet::sliderMax >= m_Level) {
        m_Slider->setValue (m_Level);
        qDebug() << Q_FUNC_INFO << "done:" << m_Slider->value();
    }
    qDebug() << Q_FUNC_INFO << "done:" << m_Slider->value();
}

/*!
 * This slot is called when the user toggled the vibration switch. The function
 * will update the UI (on/off label) and send a signal.
 */
void
ProfileContainer::slotVibrationSwitchToggled (
        bool toggle)
{
    QString text = toggle ? 
        //% "On"
        qtTrId ("qtn_comm_settings_on") :
        //% "Off"
        qtTrId ("qtn_comm_settings_off");

    m_OnOffLabel->setText (text);

    emit vibrationChanged (toggle);
}


/*!
 * This slot is called when the volume slider value has been changed. The slot
 * is used to modify the icon that shows if the volume is muted. 
 */
void
ProfileContainer::slotSliderValueChanged (
        int newValue)
{
    if (!m_Img)
        return;

    if (newValue == 0 && m_Level != 0) {
        m_Img->setImage ("icon-m-common-volume-off");
    } else if (newValue != 0 && m_Level == 0) {
        m_Img->setImage ("icon-m-common-volume");
    }

    m_Level = newValue;
}

/*!
 * This function is called when the vibration enabled/disabled. It will only 
 * modify the button state.
 */
void 
ProfileContainer::setVibration (
        bool enabled)
{
    qDebug() << Q_FUNC_INFO << "for " << title() << ":" << enabled;
    m_VibrationSwitch->setChecked (enabled);
    
    QString text = enabled ? 
        //% "On"
        qtTrId ("qtn_comm_settings_on") : 
        //% "Off"
        qtTrId ("qtn_comm_settings_off");
        
    SYS_DEBUG ("Setting on/off label to '%s'", SYS_STR(text));
    m_OnOffLabel->setText (text);
}

int 
ProfileContainer::id ()
{
    return m_ProfileId;
}

void
ProfileContainer::retranslateUi ()
{
    SYS_DEBUG ("");

    // This reloads the on/off label translation
    setVibration (m_VibrationSwitch->isChecked ());

    //% "Vibration"
    m_VibrationLabel->setText (qtTrId ("qtn_prof_vibration"));

    if (m_NoRingLabel) // Its an optional item
    {
        //% "No ringing"
        m_NoRingLabel->setText (qtTrId ("qtn_prof_noring"));
    }
}

