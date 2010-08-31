/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */

#ifndef PROFILEDATAINTERFACE_H
#define PROFILEDATAINTERFACE_H

#include <QObject>
#include <QPair>
class Profile;

class ProfileDataInterface : public QObject
{
    Q_OBJECT

public:
    
    enum ProfileId {
        ProfileIdRinging = 0,
        ProfileIdSilent,
        ProfileIdBeep,
        ProfileIdLoud,
        // none must be last to teel how many profiles there are available
        ProfileIdNone 
    };

    struct ProfileData {
        ProfileId                 profileId;
        /* IconId, profilenam(translated) pair */
        QPair<QString, QString>   visualData;
        bool                      vibrationEnabled;
        bool                      isActive;
        int                       volumeLevel;
    };

    ProfileDataInterface ();
    virtual ~ProfileDataInterface ();

    static QString mapId (int id);

signals:
    void currentProfile (int id);
    void vibrationValue (int id, bool value);

public:
    // This method has been remoed because the UI specification has been
    // changed. Will remove it later. (Can't trust the UI spec will not be
    // changed again...)
    //QString getCurrentProfileName ();
    QString getCurrentProfileIconId ();
    int getCurrentProfile ();
    QList<ProfileData> getProfilesData ();
    void setVibration (int id, bool value);

public slots:
    void setProfile (int value);

private slots:
    void currentProfileNameChanged (const QString &prof);

private:
    static ProfileId mapId (const QString &id);
    static QString id2Name (const QString &id);
    static QString mapId2IconID (int id);

private:
    Profile *m_ProfileAPI;
};

#endif
