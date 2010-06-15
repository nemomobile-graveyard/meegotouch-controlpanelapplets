/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et sw=4 ts=4 sts=4: */

#ifndef UT_PROFILEAPPLET_H
#define UT_PROFILEAPPLET_H

#include <QtTest/QtTest>
#include <QObject>

class ProfileApplet;
class MApplication;

class Ut_ProfileApplet : public QObject 
{
Q_OBJECT

private slots:
    void init ();
    void cleanup ();
    void initTestCase ();
    void cleanupTestCase ();

    void testTitle ();
    void testConstructWidget ();
    void testMenuItems ();
    void testConstructbrief ();

private:
    MApplication              *m_App;
    ProfileApplet           *m_Applet;
};

#endif


