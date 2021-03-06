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

#include "subversioncontrol.h"
#include "subversionplugin.h"

using namespace Subversion;
using namespace Subversion::Internal;

SubversionControl::SubversionControl(SubversionPlugin *plugin) :
    m_enabled(true),
    m_plugin(plugin)
{
}

QString SubversionControl::name() const
{
    return QLatin1String("subversion");
}

bool SubversionControl::isEnabled() const
{
     return m_enabled;
}

void SubversionControl::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        emit enabledChanged(m_enabled);
    }
}

bool SubversionControl::supportsOperation(Operation operation) const
{
    bool rc = true;
    switch (operation) {
    case AddOperation:
    case DeleteOperation:
        break;
    case OpenOperation:
        rc = false;
        break;
    }
    return rc;
}

bool SubversionControl::vcsOpen(const QString & /* fileName */)
{
    // Open for edit: N/A
    return true;
}

bool SubversionControl::vcsAdd(const QString &fileName)
{
    return m_plugin->vcsAdd(fileName);
}

bool SubversionControl::vcsDelete(const QString &fileName)
{
    return m_plugin->vcsDelete(fileName);
}

bool SubversionControl::managesDirectory(const QString &directory) const
{
    return m_plugin->managesDirectory(directory);
}

QString SubversionControl::findTopLevelForDirectory(const QString &directory) const
{
    return m_plugin->findTopLevelForDirectory(directory);
}
