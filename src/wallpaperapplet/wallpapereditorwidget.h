/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */
#ifndef WALLPAPEREDITORWIDGET_H
#define WALLPAPEREDITORWIDGET_H

#include <QPointer>
#include "dcpwidget.h"

#include "wallpaperbusinesslogic.h"
#include "wallpaperitrans.h"

class MContainer;
class MLinearLayoutPolicy;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
class MAction;
class QSize;
class QPixmap;
class QPointF;
class QGestuteEvent;
class WallpaperInfoHeader;

/*!
 * -output-level debug
 * -show-object-names
 * -emulate-two-finger-gestures
 */
class WallpaperEditorWidget : public DcpWidget
{
    Q_OBJECT

public:
    WallpaperEditorWidget (
            WallpaperBusinessLogic *wallpaperBusinessLogic, 
            QGraphicsWidget        *parent = 0);
    ~WallpaperEditorWidget ();

    virtual bool pagePans () const;

    virtual void paint (
            QPainter                        *painter,
            const QStyleOptionGraphicsItem  *option,
            QWidget                         *widget = 0);

protected:
    virtual void mousePressEvent (QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent (QGraphicsSceneMouseEvent *event);
    virtual void wheelEvent (QGraphicsSceneWheelEvent *event);

    bool event (QEvent *e);
    virtual void pinchGestureEvent (
            QGestureEvent *event, 
            QPinchGesture *gesture);

private slots:
    void redrawImage ();
    void orientationChanged (M::Orientation orientation);
    void slotDoneActivated ();
    void createContent ();
    void createWidgets ();

private:
    void createActions ();
    
    QPointF toggleTitlebars (bool show);
    int  imageDX () const;
    int  imageDY () const;


private:
    QPointer<WallpaperBusinessLogic>  m_WallpaperBusinessLogic;

    WallpaperInfoHeader  *m_InfoHeader;
    QPixmap               m_bgLandscape;
    QPixmap               m_bgPortrait;
    MAction              *m_DoneAction;
    bool                  m_NoTitlebar;

    QPointF               m_LastClick;
    QPointF               m_UserOffset;

    WallpaperITrans       m_Trans;
    WallpaperITrans       m_LandscapeTrans;
    WallpaperITrans       m_PortraitTrans;
    
    qreal                 m_OriginalScaleFactor;
    bool                  m_Gesture;
};

#endif
