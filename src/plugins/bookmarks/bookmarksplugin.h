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

#ifndef BOOKMARKSPLUGIN_H
#define BOOKMARKSPLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QtCore/QObject>
#include <QtCore/QMultiMap>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

namespace Core {
class ICore;
class IEditor;
}

namespace TextEditor {
class ITextEditor;
}

namespace Bookmarks {
namespace Internal {

class BookmarkManager;

class BookmarksPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    BookmarksPlugin();
    ~BookmarksPlugin();

    static BookmarksPlugin *instance() { return m_instance; }
    static Core::ICore *core() { return m_instance->m_core; }

    bool initialize(const QStringList &arguments, QString *error_message);
    void extensionsInitialized();

public slots:
    void updateActions(int stateMask);

private slots:
    void editorOpened(Core::IEditor *editor);
    void editorAboutToClose(Core::IEditor *editor);
    void requestContextMenu(TextEditor::ITextEditor *editor,
        int lineNumber, QMenu *menu);
    void bookmarkMarginActionTriggered();

private:
    static BookmarksPlugin *m_instance;
    BookmarkManager *m_bookmarkManager;
    Core::ICore *m_core;

    QAction *m_toggleAction;
    QAction *m_prevAction;
    QAction *m_nextAction;
    QAction *m_docPrevAction;
    QAction *m_docNextAction;
    QAction *m_moveUpAction;
    QAction *m_moveDownAction;

    QAction *m_bookmarkMarginAction;
    int m_bookmarkMarginActionLineNumber;
    QString m_bookmarkMarginActionFileName;
};

} // namespace Internal
} // namespace Bookmarks

#endif // BOOKMARKS_H
