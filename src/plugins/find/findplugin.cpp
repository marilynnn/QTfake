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

#include "findplugin.h"

#include "textfindconstants.h"
#include "currentdocumentfind.h"
#include "findtoolwindow.h"
#include "searchresultwindow.h"

#include <coreplugin/actionmanager/actionmanagerinterface.h>
#include <coreplugin/actionmanager/iactioncontainer.h>
#include <coreplugin/actionmanager/icommand.h>
#include <coreplugin/coreconstants.h>

#include <utils/qtcassert.h>

#include <QtCore/qplugin.h>
#include <QtCore/QSettings>

Q_DECLARE_METATYPE(Find::IFindFilter*)

namespace {
    const int MAX_COMPLETIONS = 50;
}

using namespace Find;
using namespace Find::Internal;

FindPlugin::FindPlugin()
    : m_core(0),
    m_currentDocumentFind(0),
    m_findToolBar(0),
    m_findDialog(0),
    m_findCompletionModel(new QStringListModel(this)),
    m_replaceCompletionModel(new QStringListModel(this))
{
}

FindPlugin::~FindPlugin()
{
    delete m_currentDocumentFind;
    delete m_findToolBar;
    delete m_findDialog;
}

bool FindPlugin::initialize(const QStringList &, QString *)
{
    m_core = ExtensionSystem::PluginManager::instance()->getObject<Core::ICore>();
    setupMenu();

    m_currentDocumentFind = new CurrentDocumentFind(m_core);

    m_findToolBar = new FindToolBar(this, m_currentDocumentFind);
    m_findDialog = new FindToolWindow(this);
    SearchResultWindow *searchResultWindow = new SearchResultWindow(m_core);
    addAutoReleasedObject(searchResultWindow);
    return true;
}

void FindPlugin::extensionsInitialized()
{
    setupFilterMenuItems();
    readSettings();
}

void FindPlugin::shutdown()
{
    m_findToolBar->setParent(0);
    m_currentDocumentFind->removeConnections();
    writeSettings();
}

void FindPlugin::filterChanged()
{
    IFindFilter *changedFilter = qobject_cast<IFindFilter *>(sender());
    QAction *action = m_filterActions.value(changedFilter);
    QTC_ASSERT(changedFilter, return);
    QTC_ASSERT(action, return);
    action->setEnabled(changedFilter->isEnabled());
}

void FindPlugin::openFindFilter()
{
    QAction *action = qobject_cast<QAction*>(sender());
    QTC_ASSERT(action, return);
    IFindFilter *filter = action->data().value<IFindFilter *>();
    QTC_ASSERT(filter, return);
    QTC_ASSERT(filter->isEnabled(), return);
    QString currentFindString = (m_currentDocumentFind->isEnabled() ? m_currentDocumentFind->currentFindString() : "");
    if (!currentFindString.isEmpty())
        m_findDialog->setFindText(currentFindString);
    m_findDialog->open(filter);
}


void FindPlugin::setupMenu()
{
    Core::ActionManagerInterface *am = m_core->actionManager();
    Core::IActionContainer *medit = am->actionContainer(Core::Constants::M_EDIT);
    Core::IActionContainer *mfind = am->createMenu(Constants::M_FIND);
    medit->addMenu(mfind, Core::Constants::G_EDIT_FIND);
    mfind->menu()->setTitle(tr("&Find/Replace"));
    mfind->appendGroup(Constants::G_FIND_FILTERS);
    mfind->appendGroup(Constants::G_FIND_FLAGS);
    mfind->appendGroup(Constants::G_FIND_ACTIONS);
    QList<int> globalcontext = QList<int>() << Core::Constants::C_GLOBAL_ID;
    Core::ICommand *cmd;
    QAction *separator;
    separator = new QAction(this);
    separator->setSeparator(true);
    cmd = am->registerAction(separator, QLatin1String("Find.Sep.Flags"), globalcontext);
    mfind->addAction(cmd, Constants::G_FIND_FLAGS);
    separator = new QAction(this);
    separator->setSeparator(true);
    cmd = am->registerAction(separator, QLatin1String("Find.Sep.Actions"), globalcontext);
    mfind->addAction(cmd, Constants::G_FIND_ACTIONS);
}

void FindPlugin::setupFilterMenuItems()
{
    Core::ActionManagerInterface *am = m_core->actionManager();
    QList<IFindFilter*> findInterfaces =
        ExtensionSystem::PluginManager::instance()->getObjects<IFindFilter>();
    Core::ICommand *cmd;
    QList<int> globalcontext = QList<int>() << Core::Constants::C_GLOBAL_ID;

    Core::IActionContainer *mfind = am->actionContainer(Constants::M_FIND);
    m_filterActions.clear();
    foreach (IFindFilter *filter, findInterfaces) {
        QAction *action = new QAction(filter->name(), this);
        action->setEnabled(filter->isEnabled());
        action->setData(qVariantFromValue(filter));
        cmd = am->registerAction(action, QLatin1String("FindFilter.")+filter->name(), globalcontext);
        cmd->setDefaultKeySequence(filter->defaultShortcut());
        mfind->addAction(cmd, Constants::G_FIND_FILTERS);
        m_filterActions.insert(filter, action);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(openFindFilter()));
        connect(filter, SIGNAL(changed()), this, SLOT(filterChanged()));
    }
    m_findDialog->setFindFilters(findInterfaces);
}

Core::ICore *FindPlugin::core()
{
    return m_core;
}

QTextDocument::FindFlags FindPlugin::findFlags() const
{
    return m_findFlags;
}

void FindPlugin::setCaseSensitive(bool sensitive)
{
    setFindFlag(QTextDocument::FindCaseSensitively, sensitive);
}

void FindPlugin::setWholeWord(bool wholeOnly)
{
    setFindFlag(QTextDocument::FindWholeWords, wholeOnly);
}

void FindPlugin::setBackward(bool backward)
{
    setFindFlag(QTextDocument::FindBackward, backward);
}

void FindPlugin::setFindFlag(QTextDocument::FindFlag flag, bool enabled)
{
    bool hasFlag = hasFindFlag(flag);
    if ((hasFlag && enabled) || (!hasFlag && !enabled))
        return;
    if (enabled)
        m_findFlags |= flag;
    else
        m_findFlags &= ~flag;
    if (flag != QTextDocument::FindBackward)
        emit findFlagsChanged();
}

bool FindPlugin::hasFindFlag(QTextDocument::FindFlag flag)
{
    return m_findFlags & flag;
}

void FindPlugin::writeSettings()
{
    QSettings *settings = ExtensionSystem::PluginManager::instance()->getObject<Core::ICore>()->settings();
    settings->beginGroup("Find");
    settings->setValue("Backward", QVariant((m_findFlags & QTextDocument::FindBackward) != 0));
    settings->setValue("CaseSensitively", QVariant((m_findFlags & QTextDocument::FindCaseSensitively) != 0));
    settings->setValue("WholeWords", QVariant((m_findFlags & QTextDocument::FindWholeWords) != 0));
    settings->setValue("FindStrings", m_findCompletions);
    settings->setValue("ReplaceStrings", m_replaceCompletions);
    settings->endGroup();
    m_findDialog->writeSettings();
}

void FindPlugin::readSettings()
{
    QSettings *settings = ExtensionSystem::PluginManager::instance()->getObject<Core::ICore>()->settings();
    settings->beginGroup("Find");
    bool block = blockSignals(true);
    setBackward(settings->value("Backward", false).toBool());
    setCaseSensitive(settings->value("CaseSensitively", false).toBool());
    setWholeWord(settings->value("WholeWords", false).toBool());
    blockSignals(block);
    m_findCompletions = settings->value("FindStrings").toStringList();
    m_replaceCompletions = settings->value("ReplaceStrings").toStringList();
    m_findCompletionModel->setStringList(m_findCompletions);
    m_replaceCompletionModel->setStringList(m_replaceCompletions);
    settings->endGroup();
    m_findDialog->readSettings();
    emit findFlagsChanged(); // would have been done in the setXXX methods above
}

void FindPlugin::updateFindCompletion(const QString &text)
{
    updateCompletion(text, m_findCompletions, m_findCompletionModel);
}

void FindPlugin::updateReplaceCompletion(const QString &text)
{
    updateCompletion(text, m_replaceCompletions, m_replaceCompletionModel);
}

void FindPlugin::updateCompletion(const QString &text, QStringList &completions, QStringListModel *model)
{
    if (text.isEmpty())
        return;
    completions.removeAll(text);
    completions.prepend(text);
    while (completions.size() > MAX_COMPLETIONS)
        completions.removeLast();
    model->setStringList(completions);
}

Q_EXPORT_PLUGIN(FindPlugin)
