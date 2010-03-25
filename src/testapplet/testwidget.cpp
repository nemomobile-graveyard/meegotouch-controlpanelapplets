/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */

#include "testwidget.h"

#include <QGraphicsLinearLayout>
#include <QStringList>
#include <DuiButton>
#include <DuiLabel>
#include <DuiLayout>
#include <DuiGridLayoutPolicy>
#include <DuiContainer>

#define DEBUG
#include "../debug.h"

static const int MaxColumns = 2;

TestWidget::TestWidget (
        int                 numId,
        QGraphicsWidget    *parent) :
    DcpWidget (parent),
    m_NumId (numId)
{
    createWidgets ();
    retranslateUi ();
    readLocalThemes ();
}

TestWidget::~TestWidget ()
{
}

void
TestWidget::createWidgets ()
{
    QGraphicsLinearLayout *mainLayout;
    DuiLabel              *tLabel;
    char title[512];

    sprintf (title, "This is page %d at %p", m_NumId, this);

    //DuiLayout *layout = new DuiLayout()
    tLabel = new DuiLabel (title);
    
    m_LocalContainer = createContainer ();

    mainLayout = new QGraphicsLinearLayout (Qt::Vertical);

    mainLayout->addItem (tLabel);
    mainLayout->addItem (m_LocalContainer);

    this->setLayout (mainLayout);
}

DuiContainer * 
TestWidget::createContainer ()
{
    DuiGridLayoutPolicy *policy;
    DuiLayout *layout = new DuiLayout();
    
    policy = new DuiGridLayoutPolicy (layout);
    layout->setPolicy (policy);
    
    DuiContainer *container = new DuiContainer ();
    container->centralWidget()->setLayout (layout);

    m_LocalLayoutPolicy = policy;

    return container;
}

void
TestWidget::retranslateUi ()
{
    if (m_LocalContainer) {
        m_LocalContainer->setTitle ("Activate page");
    }
}

void
TestWidget::readLocalThemes ()
{
    Q_ASSERT (m_LocalLayoutPolicy != 0);
    QStringList titleList;
    
    titleList <<
        "Page 0" <<
        "Page 1" <<
        "Page 2" <<
        "Page 3" <<
        "Page 4" <<
        "Page 5";


    int n = 0;
    foreach (QString title, titleList) {
        DuiButton *button;
        int x = n / MaxColumns;
        int y = n % MaxColumns;

        button = new DuiButton (title);
        m_LocalLayoutPolicy->addItem (button, x, y);

        connect (button, SIGNAL(clicked()),
                this, SLOT(pagingClicked()));

        ++n;
    }
}


void 
TestWidget::pagingClicked ()
{
    DuiButton *button = qobject_cast<DuiButton*> (sender());
    QString label;
    int Id;

    if (button == 0) {
        SYS_WARNING ("This is not a button?");
    }

    label = button->text();
    SYS_DEBUG ("button '%s' clicked", SYS_STR(label));

    Id = -1;
    if (label == "Page 0")
        Id = 0;
    else if (label == "Page 1")
        Id = 1;
    else if (label == "Page 2")
        Id = 2;
    else if (label == "Page 3")
        Id = 3;
    else if (label == "Page 4")
        Id = 4;
    else if (label == "Page 5")
        Id = 5;

    if (Id == -1) {
        SYS_WARNING ("Label not found");
        return;
     }

    SYS_DEBUG ("Emitting changeWidget(%d)", Id);
    emit changeWidget (Id);
}
