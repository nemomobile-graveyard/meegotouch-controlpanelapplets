/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */
#ifndef WARRANTYWIDGET_H
#define WARRANTYWIDGET_H

#include <QPointer>
#include "dcpwidget.h"

#include "warrantybusinesslogic.h"

class WarrantyWidget : public DcpWidget
{
    Q_OBJECT

public:
    WarrantyWidget (
            WarrantyBusinessLogic  *warrantyBusinessLogic, 
            QGraphicsWidget        *parent = 0);
    ~WarrantyWidget ();

    void createContent ();
    QString labelText();

private:
    QPointer<WarrantyBusinessLogic>    m_WarrantyBusinessLogic;
};

#endif