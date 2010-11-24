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

#ifndef _ALERT_TONE_TOPLEVEL_H_
#define _ALERT_TONE_TOPLEVEL_H_

#include <MAction>
#ifdef RUN_STANDALONE
#include <MApplicationPage>
#else /* !RUN_STANDALONE */
#include "dcpwidget.h"
#endif /* RUN_STANDALONE */

class AlertToneToplevel :
#ifdef RUN_STANDALONE
public MApplicationPage
#else /* !RUN_STANDALONE */
public DcpWidget
#endif /* RUN_STANDALONE */

{
	Q_OBJECT

public:
	AlertToneToplevel(QGraphicsWidget *parent = 0);

	void setTitle(QString str);
	QString title() const;

	void setViewMenuItems(QVector<MAction *>viewMenuItems);
	QVector<MAction *>viewMenuItems() const;

	void emit_changeWidget(int widgetId);

#ifdef RUN_STANDALONE
Q_SIGNALS:
	void changeWidget(int widgetId);
#endif /* RUN_STANDALONE */

private:
	QString m_title;
	QVector<MAction *> m_viewMenuItems;
};

#endif /* !_ALERT_TONE_TOPLEVEL_H_ */