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

#include "basetextmark.h"

#include <coreplugin/editormanager/editormanager.h>
#include <extensionsystem/pluginmanager.h>
#include <coreplugin/icore.h>

#include <QtCore/QTimer>

using namespace TextEditor;
using namespace TextEditor::Internal;

BaseTextMark::BaseTextMark()
    : m_markableInterface(0), m_internalMark(0), m_init(false)
{
}

BaseTextMark::BaseTextMark(const QString &filename, int line)
    : m_markableInterface(0), m_internalMark(0), m_fileName(filename), m_line(line), m_init(false)
{
    // Why is this?
    QTimer::singleShot(0, this, SLOT(init()));
}

void BaseTextMark::init()
{
    m_init = true;
    Core::EditorManager *em = ExtensionSystem::PluginManager::instance()->getObject<Core::ICore>()->editorManager();
    connect(em, SIGNAL(editorOpened(Core::IEditor *)), this, SLOT(editorOpened(Core::IEditor *)));

    foreach (Core::IEditor *editor, em->openedEditors())
        editorOpened(editor);
}

void BaseTextMark::editorOpened(Core::IEditor *editor)
{
    if (editor->file()->fileName() != m_fileName)
        return;
    if (ITextEditor *textEditor = qobject_cast<ITextEditor *>(editor)) {
        if (m_markableInterface == 0) { // We aren't added to something
            m_markableInterface = textEditor->markableInterface();
            m_internalMark = new InternalMark(this);
            m_markableInterface->addMark(m_internalMark, m_line);
        }
    }
}

void BaseTextMark::childRemovedFromEditor(InternalMark *mark)
{
    Q_UNUSED(mark)
    // m_internalMark was removed from the editor
    delete m_internalMark;
    m_markableInterface = 0;
    m_internalMark = 0;
    removedFromEditor();
}

void BaseTextMark::documentClosingFor(InternalMark *mark)
{
    Q_UNUSED(mark)
    // the document is closing
    delete m_internalMark;
    m_markableInterface = 0;
    m_internalMark = 0;
}

BaseTextMark::~BaseTextMark()
{
    // oha we are deleted
    if (m_markableInterface)
        m_markableInterface->removeMark(m_internalMark);
    delete m_internalMark;
    m_internalMark = 0;
    m_markableInterface = 0;
}

//#include <QDebug>

void BaseTextMark::updateMarker()
{
    //qDebug()<<"BaseTextMark::updateMarker()"<<m_markableInterface<<m_internalMark;
    if (m_markableInterface)
        m_markableInterface->updateMark(m_internalMark);
}

void BaseTextMark::moveMark(const QString & /* filename */, int /* line */)
{
    Core::EditorManager *em = ExtensionSystem::PluginManager::instance()->getObject<Core::ICore>()->editorManager();
    if (!m_init) {
        connect(em, SIGNAL(editorOpened(Core::IEditor *)), this, SLOT(editorOpened(Core::IEditor *)));
        m_init = true;
    }


    if (m_markableInterface)
        m_markableInterface->removeMark(m_internalMark);
    m_markableInterface = 0;
    // This is only necessary since m_internalMark is created in ediorOpened
    delete m_internalMark;
    m_internalMark = 0;

    foreach (Core::IEditor *editor, em->openedEditors())
        editorOpened(editor);
}
