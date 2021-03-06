/***************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact:  Qt Software Information (qt-info@nokia.com)
**
**
** Non-Open Source Usage
**
** Licensees may use this file in accordance with the Qt Beta Version
** License Agreement, Agreement version 2.2 provided with the Software or,
** alternatively, in accordance with the terms contained in a written
** agreement between you and Nokia.
**
** GNU General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the packaging
** of this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
**
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt GPL Exception
** version 1.3, included in the file GPL_EXCEPTION.txt in this package.
**
***************************************************************************/

#ifndef COMMAND_H
#define COMMAND_H

#include "icommand.h"
#include "actionmanager.h"

#include <QtCore/QList>
#include <QtCore/QMultiMap>
#include <QtCore/QPointer>
#include <QtGui/QKeySequence>

namespace Core {
namespace Internal {

class Command : public Core::ICommand
{
    Q_OBJECT
public:
    enum CommandState {
        CS_PreLocation      = 0x020000,
        CS_LocationChanged  = 0x040000,
        CS_Initialized      = 0x080000,
        CS_Mask             = 0xFF0000
    };

    Command(CommandType type, int id);
    virtual ~Command() {}

    void setStateFlags(int state);
    int stateFlags() const;

    virtual QString name() const = 0;

    void setCategory(const QString &name);
    QString category() const;

    void setDefaultKeySequence(const QKeySequence &key);
    QKeySequence defaultKeySequence() const;

    void setDefaultText(const QString &text);
    QString defaultText() const;

    int id() const;
    CommandType type() const;

    QAction *action() const;
    QShortcut *shortcut() const;

    void setAttribute(CommandAttribute attr);
    void removeAttribute(CommandAttribute attr);
    bool hasAttribute(CommandAttribute attr) const;

    virtual bool setCurrentContext(const QList<int> &context) = 0;

    QString stringWithAppendedShortcut(const QString &str) const;

protected:
    QString m_category;
    int m_type;
    int m_id;
    QKeySequence m_defaultKey;
    QString m_defaultText;
};

class Shortcut : public Command
{
    Q_OBJECT
public:
    Shortcut(int id);

    QString name() const;

    void setDefaultKeySequence(const QKeySequence &key);
    void setKeySequence(const QKeySequence &key);
    QKeySequence keySequence() const;

    virtual void setDefaultText(const QString &key);
    virtual QString defaultText() const;

    void setShortcut(QShortcut *shortcut);
    QShortcut *shortcut() const;

    void setContext(const QList<int> &context);
    QList<int> context() const;
    bool setCurrentContext(const QList<int> &context);

    bool isActive() const;
private:
    QList<int> m_context;
    QShortcut *m_shortcut;
    QString m_defaultText;
};

class Action : public Command
{
    Q_OBJECT
public:
    Action(CommandType type, int id);

    QString name() const;

    void setDefaultKeySequence(const QKeySequence &key);
    void setKeySequence(const QKeySequence &key);
    QKeySequence keySequence() const;

    virtual void setAction(QAction *action);
    QAction *action() const;

    void setLocations(const QList<CommandLocation> &locations);
    QList<CommandLocation> locations() const;

protected:
    void updateToolTipWithKeySequence();
    
    QAction *m_action;
    QList<CommandLocation> m_locations;
    QString m_toolTip;
};

class OverrideableAction : public Action
{
    Q_OBJECT

public:
    OverrideableAction(int id);

    void setAction(QAction *action);
    bool setCurrentContext(const QList<int> &context);
    void addOverrideAction(QAction *action, const QList<int> &context);
    bool isActive() const;

private slots:
    void actionChanged();

private:
    QPointer<QAction> m_currentAction;
    QList<int> m_context;
    QMap<int, QPointer<QAction> > m_contextActionMap;
    bool m_active;
    bool m_contextInitialized;
};

} // namespace Internal
} // namespace Core

#endif // COMMAND_H
