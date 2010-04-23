/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */
/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */

#include "wallpapereditorwidget.h"

#include <QStyle>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <QPixmap>
#include <QTimer>

#include <MLayout>
#include <MLinearLayoutPolicy>

#include <MAction>
#include <MApplicationWindow>
#include <MApplication>
#include <MApplicationPage>

#include "mwidgetcreator.h"
M_REGISTER_WIDGET_NO_CREATE(WallpaperEditorWidget)

#define DEBUG
#include "../debug.h"

static const int ExtraMargin = 10;

WallpaperEditorWidget::WallpaperEditorWidget (
        WallpaperBusinessLogic *wallpaperBusinessLogic, 
        QGraphicsWidget        *parent) :
    DcpWidget (parent),
    m_WallpaperBusinessLogic (wallpaperBusinessLogic),
    m_bgLandscape (0),
    m_bgPortrait (0),
    m_DoneAction (0),
    m_NoTitlebar (false),
    m_Scale (1.0)
{
    m_LandscapeSize = MApplication::activeWindow ()->visibleSceneSize (
            M::Landscape);
    MApplication::activeApplicationWindow()->showFullScreen();
    
    setObjectName ("WallpaperEditorWidget");
    QTimer::singleShot (0, this, SLOT(createContent()));
    createActions ();

    connect(MApplication::activeApplicationWindow(),
            SIGNAL(orientationChanged(M::Orientation)),
            this, SLOT(orientationChanged(M::Orientation)));
}

WallpaperEditorWidget::~WallpaperEditorWidget ()
{
    MApplication::activeApplicationWindow()->showNormal();
}

void
WallpaperEditorWidget::paint (
        QPainter                          *painter,
        const QStyleOptionGraphicsItem    *option,
        QWidget                           *widget)
{
    bool portrait = (geometry().height() > geometry().width());

    if ((m_bgPortrait != 0) && portrait) {
        painter->drawPixmap (geometry().toRect(), *m_bgPortrait);
    } else if (m_bgLandscape && ! portrait) {
        painter->fillRect (
                -ExtraMargin, -ExtraMargin, 
                m_LandscapeSize.width(),
                m_LandscapeSize.height(),
                QColor ("black"));

        painter->drawPixmap (
                imageDX(), imageDY(),
                m_LandscapeSize.width() * m_Scale,
                m_LandscapeSize.height() * m_Scale,
                *m_bgLandscape);
    }

    DcpWidget::paint (painter, option, widget);
}

void
WallpaperEditorWidget::createContent ()
{
    QString filename = m_WallpaperBusinessLogic->editedImage()->filename();

    SYS_WARNING ("*** filename = %s", SYS_STR(filename));

    /*
     * If the image is very big the handling might be slow, so we  scale it
     * down.
     */
    QPixmap toCheck (filename);
    m_bgLandscape = new QPixmap (toCheck.scaled (m_LandscapeSize));

    SYS_DEBUG ("size = %dx%d", 
            m_LandscapeSize.width(), 
            m_LandscapeSize.height());

    //this->setContentsMargins (0, 0, 0, 0);
    //this->setWindowFrameMargins (0.0, 0.0, 0.0, 0.0);

    this->setMinimumSize (m_LandscapeSize);

    /*
     * We need to update the current page and not just this widget because of
     * those damn extra margins coming from somewhere.
     */
    MApplication::activeApplicationWindow()->currentPage()->update();
}

/*
 * In this function we create the actions (currently only the 'done' action that
 * is shown in the toolbar. Unfortunatelly the 'done' action can not be added to
 * a widget (it is not shown in the toolbar if added to a widget I guess because
 * the widget is not added to the window yet), and it also can not be added to
 * the current page (the new page for this widget is not created yet), so we
 * have to get the application window and add the action to that. 
 */
void
WallpaperEditorWidget::createActions ()
{
    MApplicationWindow *window = 
        MApplication::instance()->activeApplicationWindow();

    m_DoneAction = new MAction(
            "icon-m-framework-done",
            //% "Done"
            qtTrId("qtn_wall_done"), 
            this);
    m_DoneAction->setLocation(MAction::ToolBarLocation);
    window->addAction(m_DoneAction);
    m_DoneAction->setVisible(true);

    connect(m_DoneAction, SIGNAL(triggered()), 
            this, SLOT(slotDoneActivated()));
}

/*
 * This slot is called when the user activates the 'done' action and so we have
 * to store the settings.
 */
void
WallpaperEditorWidget::slotDoneActivated ()
{
    SYS_DEBUG ("");
    m_WallpaperBusinessLogic->setBackground();
    changeWidget (0);
}

bool 
WallpaperEditorWidget::pagePans () const
{
    return false;
}

void 
WallpaperEditorWidget::mouseMoveEvent (
        QGraphicsSceneMouseEvent *event)
{
    SYS_DEBUG ("-----------------------------------------------------");
    SYS_DEBUG ("*** button()      = %d", event->button());
    SYS_DEBUG ("*** pos()         = %f, %f", event->pos().x(), event->pos().y());
    SYS_DEBUG ("*** lastClick     = %f, %f", m_LastClick.x(), m_LastClick.y());
    m_UserOffset = event->pos() - m_LastClick;

    SYS_DEBUG ("*** userOffset    = %f, %f", m_UserOffset.x(), m_UserOffset.y());
    SYS_DEBUG ("*** OlduserOffset = %f, %f", m_OldUserOffset.x(), m_OldUserOffset.y());

    /*
     * We need to update the current page and not just this widget because of
     * those damn extra margins coming from somewhere.
     */
    MApplication::activeApplicationWindow()->currentPage()->update();
}

void 
WallpaperEditorWidget::wheelEvent (
        QGraphicsSceneWheelEvent *event)
{
    SYS_DEBUG ("*** delta = %d", event->delta());
    m_Scale = m_Scale + (event->delta() / 1200.0);
    if (m_Scale < 0.1)
        m_Scale = 0.1;
    
    SYS_DEBUG ("*** m_Scale = %f", m_Scale);
    /*
     * We need to update the current page and not just this widget because of
     * those damn extra margins coming from somewhere.
     */
    MApplication::activeApplicationWindow()->currentPage()->update();
}

void
WallpaperEditorWidget::mousePressEvent (
        QGraphicsSceneMouseEvent *event)
{
    MApplicationPage  *currentPage;

    SYS_DEBUG ("");
    currentPage = MApplication::activeApplicationWindow()->currentPage();
    currentPage->setComponentsDisplayMode (
            MApplicationPage::AllComponents,
            MApplicationPageModel::Hide);
    m_LastClick = event->pos();

    /*
     * FIXME: We need this, because we just hide the titlebar.
     */
    m_LastClick += QPointF (0, 70);
    m_NoTitlebar = true;
}

void
WallpaperEditorWidget::mouseReleaseEvent (
        QGraphicsSceneMouseEvent *event)
{
    MApplicationPage  *currentPage;

    SYS_DEBUG ("");
    currentPage = MApplication::activeApplicationWindow()->currentPage();
    currentPage->setComponentsDisplayMode (
            MApplicationPage::AllComponents,
            MApplicationPageModel::Show);

    m_OldUserOffset += m_UserOffset;
    m_UserOffset = QPointF();
    m_NoTitlebar = false;
}

/*!
 * Returns the X offset where the image should be painted inside the widget. 
 */
int
WallpaperEditorWidget::imageDX () const
{
    int retval = 0;

    retval -= ExtraMargin;

    retval += m_UserOffset.x();
    retval += m_OldUserOffset.x();
    return retval;
}

/*!
 * Returns the Y offset where the image should be painted inside the widget. 
 */
int
WallpaperEditorWidget::imageDY () const
{
    int                 retval = 0;

    #if 0
    /*
     * Trying to find where the margins are coming from.
     */
    MApplicationPage   *currentPage;
    MApplicationWindow *window;
    qreal               topMargin, leftMargin;
    int                 wTopMargin, wLeftMargin;
    currentPage = MApplication::activeApplicationWindow()->currentPage();
    window = MApplication::activeApplicationWindow();

    getWindowFrameMargins (&leftMargin, &topMargin, 0, 0);
    SYS_DEBUG ("*** windowFrameMargins = %f, %f", leftMargin, topMargin);

    getContentsMargins (&leftMargin, &topMargin, 0, 0);
    SYS_DEBUG ("*** getContentsMargins = %f, %f", leftMargin, topMargin);

    currentPage->getWindowFrameMargins (&leftMargin, &topMargin, 0, 0);
    SYS_DEBUG ("*** windowFrameMargins = %f, %f", leftMargin, topMargin);

    currentPage->getContentsMargins (&leftMargin, &topMargin, 0, 0);
    SYS_DEBUG ("*** getContentsMargins = %f, %f", leftMargin, topMargin);
    
    window->getContentsMargins (&wLeftMargin, &wTopMargin, 0, 0);
    SYS_DEBUG ("*** getContentsMargins = %d, %d", wLeftMargin, wTopMargin);
    #endif

    retval -= ExtraMargin;

    if (!m_NoTitlebar)
        retval -= 70;

    retval += m_UserOffset.y();
    retval += m_OldUserOffset.y();
    return retval;
}

void 
WallpaperEditorWidget::orientationChanged (
        M::Orientation orientation)
{
    switch (orientation) {
        case M::Portrait:
            SYS_DEBUG ("Turned to portrait");
            break;

        case M::Landscape:
            SYS_DEBUG ("Turned to landscape");
            break;
    }
}
