/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "tapandholdgesture.h"

#include <QtGui/qevent.h>

/*!
    \class TapAndHoldGesture
    \since 4.6

    \brief The TapAndHoldGesture class represents a Tap-and-Hold gesture,
    providing additional information.
*/

const int TapAndHoldGesture::iterationCount = 40;
const int TapAndHoldGesture::iterationTimeout = 50;

/*!
    Creates a new Tap and Hold gesture handler object and marks it as a child
    of \a parent.

    On some platforms like Windows there is a system-wide tap and hold gesture
    that cannot be overriden, hence the gesture might never trigger and default
    context menu will be shown instead.
*/
TapAndHoldGesture::TapAndHoldGesture(QWidget *parent)
    : QGesture(parent), iteration(0)
{
}

/*! \internal */
bool TapAndHoldGesture::filterEvent(QEvent *event)
{
    if (!event->spontaneous())
        return false;
    const QTouchEvent *ev = static_cast<const QTouchEvent*>(event);
    switch (event->type()) {
    case QEvent::TouchBegin: {
        if (timer.isActive())
            timer.stop();
        timer.start(TapAndHoldGesture::iterationTimeout, this);
        const QPoint p = ev->touchPoints().at(0).pos().toPoint();
        position = p;
        break;
    }
    case QEvent::TouchUpdate:
        if (ev->touchPoints().size() == 1) {
            const QPoint startPos = ev->touchPoints().at(0).startPos().toPoint();
            const QPoint pos = ev->touchPoints().at(0).pos().toPoint();
            if ((startPos - pos).manhattanLength() > 15)
                reset();
        } else {
            reset();
        }
        break;
    case QEvent::TouchEnd:
        reset();
        break;
    default:
        break;
    }
    return false;
}

/*! \internal */
void TapAndHoldGesture::timerEvent(QTimerEvent *event)
{
    if (event->timerId() != timer.timerId())
        return;
    if (iteration == TapAndHoldGesture::iterationCount) {
        timer.stop();
        setState(Qt::GestureFinished);
        emit triggered();
    } else {
        setState(Qt::GestureStarted);
        emit triggered();
    }
    ++iteration;
}

/*! \internal */
void TapAndHoldGesture::reset()
{
    if (state() != Qt::NoGesture)
        emit cancelled();
    setState(Qt::NoGesture);
    timer.stop();
    iteration = 0;
}

/*!
    \property TapAndHoldGesture::pos

    \brief The position of the gesture.
*/
QPoint TapAndHoldGesture::pos() const
{
    return position;
}