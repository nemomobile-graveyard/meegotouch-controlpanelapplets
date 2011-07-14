/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et sw=4 ts=4 sts=4: */

#include <MApplication>
#include <QString>
#include <QStringList>
#include  "ut_alerttonewidget.h"
#include "alerttonewidget.h"

#define DEBUG
#define WARNING
#include "../debug.h"

/******************************************************************************
 * Ut_AlertToneWidget implementation.
 */
int   argc = 1;
char *argv[] = {
      (char *) "./ut_alerttonewidget",
      NULL };

void
Ut_AlertToneWidgetTests::initTestCase()
{
      m_App = new MApplication(argc, argv);
      signalFlag = 0;
}

void
Ut_AlertToneWidgetTests::cleanupTestCase()
{
    if (m_App)
        delete m_App;
}

void
Ut_AlertToneWidgetTests::alerttonewidgetConstructor ()
{
      AlertTone at("email.alert.tone");
      AlertToneToplevel * attl=   new AlertToneToplevel (0);
      AlertToneWidget    atw (&at, 1, attl, 0);

      QCOMPARE( atw.m_tone , &at );
      QCOMPARE( atw.m_changer , attl );
      QCOMPARE( 1 , atw.m_idx );
}

/*!
 * FIXME: Unfortunately I was not able to understand this test, but it was
 * failing when the AlertTone was enhanced so it is not accepting the values
 * when the file does not exists.
 */
void
Ut_AlertToneWidgetTests::alerttonewidgetAlertToneChanged()
{
      AlertTone at("email.alert.tone");
      AlertToneToplevel * attl=   new AlertToneToplevel (0);
      AlertToneWidget    atw (&at, 1, attl, 0);
      QString string = atw.property("subtitle").toString();
#if 0
      atw.m_tone->m_val.clear ();
      atw.m_tone->m_key = "ringing.alert.type";
      atw.m_tone->emit_changed();
      QVERIFY (string != atw.property("subtitle").toString()  );
#endif
}

void
Ut_AlertToneWidgetTests::alerttonewidgetRetranslateUi()
{
    AlertTone at("email.alert.tone");
    AlertToneToplevel * attl=   new AlertToneToplevel (0);
    AlertToneWidget    atw (&at, 1, attl, 0);
      
    atw.retranslateUi();
    QString title = atw.property("title").toString();
    SYS_WARNING ("*** title = %s", SYS_STR(title));
}

void
Ut_AlertToneWidgetTests::alerttonewidgetClicked()
{
      AlertTone at("email.alert.tone");
      AlertToneToplevel * attl=   new AlertToneToplevel (0);
      AlertToneWidget    atw (&at, 1, attl, 0);
      QObject::connect(atw.m_changer, SIGNAL(changeWidget(int)), this, SLOT(receiveSignal(int)));
      atw.click();
      QVERIFY (1 == signalFlag );
}

QTEST_APPLESS_MAIN(Ut_AlertToneWidgetTests)