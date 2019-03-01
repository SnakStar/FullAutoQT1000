﻿/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore>

#include "myinputpanelcontext.h"

//! [0]

MyInputPanelContext::MyInputPanelContext()
{
    inputPanel = new MyInputPanel;
    //connect(inputPanel, SIGNAL(characterGenerated(QChar)), SLOT(sendCharacter(QChar)));
    //槽函数将按下的值显示出来
    connect(inputPanel, SIGNAL(characterGenerated(int)), SLOT(sendCharacter(int)));
}

//! [0]

MyInputPanelContext::~MyInputPanelContext()
{
    delete inputPanel;
}

//! [1]

bool MyInputPanelContext::filterEvent(const QEvent* event)
{
    if (event->type() == QEvent::RequestSoftwareInputPanel) {
        updatePosition();
        inputPanel->show();
        return true;
    } else if (event->type() == QEvent::CloseSoftwareInputPanel) {
        inputPanel->hide();
        return true;
    }
    return false;
}

//! [1]

QString MyInputPanelContext::identifierName()
{
    return "MyInputPanelContext";
}

void MyInputPanelContext::reset()
{
}

void MyInputPanelContext::showInputPanel()
{
    updatePosition();
    inputPanel->show();
}

bool MyInputPanelContext::isComposing() const
{
    return false;
}

QString MyInputPanelContext::language()
{
    return "en_US";
}

//! [2]

void MyInputPanelContext::sendCharacter(int key)
{
    //QPointer<QWidget> w = focusWidget();
    QPointer<QWidget> w = inputPanel->getFocusedWidget();
    //判断焦点是否在主界面上
    if (!w)
        return;

    //QKeyEvent keyPress(QEvent::KeyPress, key.unicode(), Qt::NoModifier, QString(key));
    QKeyEvent keyPress(QEvent::KeyPress, key, Qt::NoModifier, QString(key));
    QApplication::sendEvent(w, &keyPress);

    if (!w)
        return;

    //QKeyEvent keyRelease(QEvent::KeyPress, key.unicode(), Qt::NoModifier, QString());
    //检测 按键被释放,这里一定要用“QEvent::KeyRelease”，不然删除键会执行两次。
    QKeyEvent keyRelease(QEvent::KeyRelease, key, Qt::NoModifier, QString());
    QApplication::sendEvent(w, &keyRelease);
}

//! [2]

//! [3]

void MyInputPanelContext::updatePosition()
{
    //QWidget *widget = focusWidget();
    QWidget* widget = inputPanel->getFocusedWidget();
    if (!widget)
        return;
    //更新显示位置
    QRect widgetRect = widget->rect();
    QPoint panelPos = QPoint(widgetRect.left(), widgetRect.bottom() + 2);
    panelPos = widget->mapToGlobal(panelPos);
    inputPanel->move(panelPos);
}

//! [3]
