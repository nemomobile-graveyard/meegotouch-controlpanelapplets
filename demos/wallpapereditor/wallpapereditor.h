/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */
#ifndef TEST_H
#define TEST_H

#include <QString>
#include <QPointer>

#include <MApplication>
#include <MButton>
#include <WallpaperEditorWidget>

class MApplicationWindow;
class MApplicationPage;

class MyApplication : public MApplication {
    Q_OBJECT
    public:
        MyApplication (int &argc, char **argv);

    public slots:
        void startEditor ();
        void closeEditor (int widgetID = 0);

    private:
        MApplicationWindow            *mainwindow;
        QPointer<MApplicationPage>     page1;
        QPointer<MApplicationPage>     page2;
        QString                        file1;
        QString                        file2;
};
#endif
