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

#include "plaintexteditor.h"
#include "texteditorconstants.h"
#include "texteditorplugin.h"

#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <coreplugin/uniqueidmanager.h>

using namespace TextEditor;
using namespace TextEditor::Internal;

PlainTextEditorEditable::PlainTextEditorEditable(PlainTextEditor *editor)
    :BaseTextEditorEditable(editor)
{
    Core::ICore *core = TextEditorPlugin::core();
    m_context << core->uniqueIDManager()->
        uniqueIdentifier(Core::Constants::K_DEFAULT_TEXT_EDITOR);
    m_context << core->uniqueIDManager()->
        uniqueIdentifier(TextEditor::Constants::C_TEXTEDITOR);
}

PlainTextEditor::PlainTextEditor(QWidget *parent) :
    BaseTextEditor(parent)
{

    setRevisionsVisible(true);
    setMarksVisible(true);
    setRequestMarkEnabled(false);
    setLineSeparatorsAllowed(true);

    setMimeType(QLatin1String(TextEditor::Constants::C_TEXTEDITOR_MIMETYPE_TEXT));
}

QList<int> PlainTextEditorEditable::context() const
{
    return m_context;
}


Core::IEditor *PlainTextEditorEditable::duplicate(QWidget *parent)
{
    PlainTextEditor *newEditor = new PlainTextEditor(parent);
    newEditor->duplicateFrom(editor());
    TextEditorPlugin::instance()->initializeEditor(newEditor);
    return newEditor->editableInterface();
}

const char *PlainTextEditorEditable::kind() const
{
    return Core::Constants::K_DEFAULT_TEXT_EDITOR;
}

// Indent a text block based on previous line.
// Simple text paragraph layout:
// aaaa aaaa
//
//   bbb bb
//   bbb bb
//
//  - list
//    list line2
//
//  - listn
//
// ccc
//
// @todo{Add formatting to wrap paragraphs. This requires some
// hoops as the current indentation routines are not prepared
// for additional block being inserted. It might be possible
// to do in 2 steps (indenting/wrapping)}
//

void PlainTextEditor::indentBlock(QTextDocument *doc, QTextBlock block, QChar /* typedChar */)
{
    // At beginning: Leave as is.
    if (block == doc->begin())
        return;

    const QTextBlock previous = block.previous();
    const QString previousText = previous.text();
    // Empty line indicates a start of a new paragraph. Leave as is.
    if (previousText.isEmpty() || previousText.trimmed().isEmpty())
        return;

    // Just use previous line.
    // Skip non-alphanumerical characters when determining the indentation
    // to enable writing bulleted lists whose items span several lines.
    int i = 0;
    while (i < previousText.size()) {
        if (previousText.at(i).isLetterOrNumber()) {
            const TextEditor::TabSettings &ts = tabSettings();
            ts.indentLine(block, ts.columnAt(previousText, i));
            break;
        }
        ++i;
    }
}
