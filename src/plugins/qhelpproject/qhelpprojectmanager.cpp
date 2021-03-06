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

#include "qhelpprojectmanager.h"
#include "qhelpproject.h"

#include <QtCore/qplugin.h>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>

using namespace QHelpProjectPlugin::Internal;

QHelpProjectManager::QHelpProjectManager()
{
}

QHelpProjectManager::~QHelpProjectManager()
{
}

bool QHelpProjectManager::init(ExtensionSystem::PluginManager *pm, QString *error_message)
{
    m_pm = pm;
    m_core = m_pm->interface<QWorkbench::ICore>();
    QWorkbench::ActionManagerInterface *am = m_core->actionManager();

    m_projectContext = m_core->uniqueIDManager()->
        uniqueIdentifier(QLatin1String("QHelpProject"));

    m_languageId = m_core->uniqueIDManager()->
        uniqueIdentifier(QLatin1String("QHelpLanguage"));

    m_projectExplorer = m_pm->interface<ProjectExplorer::ProjectExplorerInterface>();

    return true;
}

void QHelpProjectManager::extensionsInitialized()
{
}

void QHelpProjectManager::cleanup()
{
}

int QHelpProjectManager::projectContext() const
{
    return m_projectContext;
}

int QHelpProjectManager::projectLanguage() const
{
    return m_languageId;
}

bool QHelpProjectManager::canOpen(const QString &fileName)
{
    qDebug() << "can open " << fileName;
    QFileInfo fi(fileName);
    if (fi.suffix() == QLatin1String("qthp"))
        return true;
    return false;
}

QList<ProjectExplorer::Project*> QHelpProjectManager::open(const QString &fileName)
{
    QList<ProjectExplorer::Project*> lst;
    QHelpProject *pro = new QHelpProject(this);
    if (pro->open(fileName)) {
        lst.append(pro);
    } else {
        delete pro;
    }
    return lst;
}

QString QHelpProjectManager::fileFilter() const
{
    return tr("Qt Help Project File (*.qthp)");
}

QVariant QHelpProjectManager::editorProperty(const QString &key) const
{
    return QVariant();
}

ProjectExplorer::ProjectExplorerInterface *QHelpProjectManager::projectExplorer() const
{
    return m_projectExplorer;
}

Q_EXPORT_PLUGIN(QHelpProjectManager)
