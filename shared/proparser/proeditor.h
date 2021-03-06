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

#ifndef PROEDITOR_H
#define PROEDITOR_H

#include "namespace_global.h"

#include "ui_proeditor.h"

#include "proiteminfo.h"

#include <QtCore/QList>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE
class QMenu;
class QAction;
class ProBlock;
class ProVariable;
class ProFile;
QT_END_NAMESPACE

namespace Qt4ProjectManager {
namespace Internal {

class ProEditorModel;
class ProScopeFilter;

class ProEditor : public QWidget, protected Ui::ProEditor
{
    Q_OBJECT

public:
    ProEditor(QWidget *parent, bool shortcuts = true);
    ~ProEditor();

    virtual void initialize(ProEditorModel *model, ProItemInfoManager *infomanager);

    ProScopeFilter *filterModel() const;

public slots:
    void selectScope(const QModelIndex &scope);

signals:
    void itemSelected(const QModelIndex &index);

protected slots:
    void showContextMenu(const QPoint &pos);
    void updatePasteAction();
    void updateState();

    void moveUp();
    void moveDown();
    void remove();
    void cut();
    void copy();
    void paste();

    void addVariable();
    void addScope();
    void addBlock();

protected:
    void updateActions(bool focus);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void initialize();

protected:
    ProEditorModel *m_model;
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;

private:
    QMenu *m_contextMenu;

    QAction *m_addVariable;
    QAction *m_addScope;
    QAction *m_addBlock;

    ProScopeFilter *m_filter;
    ProItemInfoManager *m_infomanager;

    bool m_blockSelectionSignal;

    // used because of some strange behavior when integrated into eclipse
    bool m_setFocusToListView;
    bool m_shortcuts;
    bool m_advanced;
};

} //namespace Internal
} //namespace Qt4ProjectManager

#endif // PROEDITOR_H
