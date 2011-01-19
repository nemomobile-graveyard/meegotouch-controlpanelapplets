/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelsoundsettingsapplet.
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
#ifndef _ALERT_TONE_BROWSER_H_
#define _ALERT_TONE_BROWSER_H_

#include <QPointer>

#include "alerttone.h"
#include "alerttonepreview.h"
#include "alerttonetoplevel.h"

class QString;
class MTextEdit;
class QGraphicsLinearLayout;
class MCustomContentItem;
class MAction;
class DrillDownItem;
class AlertToneDefaults;
class MApplicationPage;

/*!
 * A compound widget that holds two buttons and a list widget. The two buttons
 * can be used to pick a sound file using a content picker page and load a sound
 * file from the OVI store. The list contains the default sound files, a list
 * that is created reading some specific directories of the file system.
 */
class AlertToneBrowser : public AlertToneToplevel
{
Q_OBJECT

public:
    AlertToneBrowser(AlertTone *tone, QGraphicsWidget *parent = 0);
    ~AlertToneBrowser();

private slots:
    void defaultItemClicked (const QString &filename);
    void defaultsDisplayEntered();
    void setAlertTone (
            const QString &fname,
            bool           setGui = false);
    void set (
        const QVariant &filename);

    void launchMusicBrowser();
    void launchOviStore();
    void selectingMusicItem(const QString &item);
    void cancel();
    void accept();
    void textChanged ();
    void browserBackButtonClicked ();
    void stopPlayingSound ();

protected:
    virtual void retranslateUi();
    virtual void polishEvent ();

private:
    void startPlayingSound (const QString &filename);
    virtual void createContent();

    AlertTone                       *m_tone;
    DrillDownItem                   *m_my_music;
    DrillDownItem                   *m_ovi_store;

    QGraphicsLinearLayout           *m_MainLayout;
    AlertToneDefaults               *m_defaults;
    MTextEdit                       *m_LiveFilterEditor;
    QString                          currSelectedFile;
    QPointer<AlertTonePreview>       m_preview;
    MApplicationPage                *m_MusicBrowser;

    MAction                         *m_DoneAction;
    MAction                         *m_CancelAction;

#ifdef UNIT_TEST
    friend class Ut_AlertToneBrowserTests;
#endif
};

#endif /* !_ALERT_TONE_BROWSER_H_ */
