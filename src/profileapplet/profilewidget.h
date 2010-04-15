/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */

#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QPointer>
#include "dcpwidget.h"
#include "profilebuttons.h"
#include "profiledatainterface.h"

class MLabel;
class MContainer;
class ProfileContainer;

class ProfileWidget : public DcpWidget
{
    Q_OBJECT

public:
    ProfileWidget (
            ProfileDataInterface *api,
            QGraphicsWidget *parent = 0);
    virtual ~ProfileWidget();

    void retranslateUi ();
    QString currentProfile();

protected:
    void initWidget();

private slots:
    void initProfiles ();
    void setProfile (int profileId);
    void setVolume (int profileId, int level);
    void setVibration (int profileId, bool enabled);

    void profileSelected (int id);
    void sliderValueChanged (int index);
    void vibrationChanged (bool enabled);

private:
    MContainer* createContainer();

private:
    QPointer<ProfileDataInterface>   m_ProfileIf;
    ProfileButtons                  *m_ProfileButtons;
    QHash<int, ProfileContainer*>    m_Containers;
    MLabel                        *m_currentHeader;
    MLabel                        *m_settingsHeader;
};
#endif
