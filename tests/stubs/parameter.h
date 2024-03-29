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
****************************************************************************/
#ifndef PARAMETER_H
#define PARAMETER_H

class ParameterBase
{
public:
    virtual ~ParameterBase() {
        // Class needs to have at least one virtual function to be polymorphic
        // (and thus enable dynamic_cast)
    }
protected:
    ParameterBase() {
    }
};

template <typename T>
class Parameter : public ParameterBase
{
public:
    Parameter(T value) : ParameterBase(), data(value) {
    }
    T data;
};
#endif
