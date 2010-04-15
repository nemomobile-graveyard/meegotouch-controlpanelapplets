/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */
#ifndef WALLPAPEREDITORWIDGET_H
#define WALLPAPEREDITORWIDGET_H

#include <QPointer>
#include "dcpwidget.h"

#include "wallpaperbusinesslogic.h"

class DuiContainer;
class DuiLinearLayoutPolicy;
class DuiButton;
class DuiImageWidget;

class WallpaperEditorWidget : public DcpWidget
{
    Q_OBJECT

public:
    WallpaperEditorWidget (
            WallpaperBusinessLogic *wallpaperBusinessLogic, 
            QGraphicsWidget        *parent = 0);
    ~WallpaperEditorWidget ();

    void retranslateUi ();

private slots:
    void slotDoneButtonClicked ();

private:
    void createWidgets ();

    QPointer<WallpaperBusinessLogic>  m_WallpaperBusinessLogic;
    DuiImageWidget                   *m_Image;
    DuiButton                        *m_DoneButton;
};

#endif
