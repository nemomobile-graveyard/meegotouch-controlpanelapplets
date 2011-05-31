/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelapplets.
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
** Contact: Karoliina T. Salminen <karoliina.t.salminen@nokia.com>
** Authors: David Kedves <dkedves@blumsoft.eu>
**          Laszlo Pere <lpere@blumsoft.eu>
**
****************************************************************************/
#ifndef WALLPAPERLISTSHEET_H
#define WALLPAPERLISTSHEET_H

#include <MSheet>
class WallpaperBusinessLogic;

class WallpaperListSheet : public MSheet
{
    Q_OBJECT

    public:
        WallpaperListSheet (
                WallpaperBusinessLogic *wallpaperBusinessLogic);
       
    protected slots:
        void workerStarted ();
        void workerEnded ();
        
    private:
        void createHeaderWidget();
        void createCentralWidget(
                WallpaperBusinessLogic *wallpaperBusinessLogic);

};

#endif
