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

#include "formeditorplugin.h"
#include "formeditorfactory.h"
#include "formeditorw.h"
#include "formwizard.h"

#ifdef CPP_ENABLED
#  include "formclasswizard.h"
#  include <cppeditor/cppeditorconstants.h>
#endif

#include "designerconstants.h"
#if QT_VERSION < 0x040500
#    include "settings.h"
#endif

#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/uniqueidmanager.h>

#include <QtCore/qplugin.h>
#include <QtCore/QDebug>

#ifdef CPP_ENABLED
#    include <QtGui/QAction>
#    include <QtGui/QWizard>
#    include <QtGui/QMainWindow>
#endif

using namespace Designer::Internal;
using namespace Designer::Constants;

FormEditorPlugin::FormEditorPlugin() :
    m_factory(0),
    m_formWizard(0),
    m_formClassWizard(0)
{
}

FormEditorPlugin::~FormEditorPlugin()
{
    if (m_factory)
        removeObject(m_factory);
    if (m_formWizard)
        removeObject(m_formWizard);
    if (m_formClassWizard)
        removeObject(m_formClassWizard);
    delete m_factory;
    delete m_formWizard;
    delete m_formClassWizard;
    FormEditorW::deleteInstance();
}

////////////////////////////////////////////////////
//
// INHERITED FROM ExtensionSystem::Plugin
//
////////////////////////////////////////////////////
bool FormEditorPlugin::initialize(const QStringList & /*arguments*/, QString *error_message/* = 0*/) // =0;
{
    Core::ICore *core = ExtensionSystem::PluginManager::instance()->getObject<Core::ICore>();
    if (!core->mimeDatabase()->addMimeTypes(QLatin1String(":/formeditor/Designer.mimetypes.xml"), error_message))
        return false;

    if (!initializeTemplates(error_message))
        return false;

    const int uid = core->uniqueIDManager()->uniqueIdentifier(QLatin1String(C_FORMEDITOR));
    const QList<int> context = QList<int>() << uid;

    m_factory = new FormEditorFactory(core);
    addObject(m_factory);

    // Make sure settings pages and action shortcuts are registered
    FormEditorW::ensureInitStage(FormEditorW::RegisterPlugins);

    error_message->clear();
    return true;
}

void FormEditorPlugin::extensionsInitialized()
{
}

////////////////////////////////////////////////////
//
// PRIVATE methods
//
////////////////////////////////////////////////////

bool FormEditorPlugin::initializeTemplates(QString * /* error_message */)
{

    Core::ICore *core = ExtensionSystem::PluginManager::instance()->getObject<Core::ICore>();
    FormWizard::BaseFileWizardParameters wizardParameters(Core::IWizard::FileWizard);
    wizardParameters.setCategory(QLatin1String("Qt"));
    wizardParameters.setTrCategory(tr("Qt"));
    const QString formFileType = QLatin1String(Constants::FORM_FILE_TYPE);
    wizardParameters.setName(tr("Qt Designer Form"));
    wizardParameters.setDescription(tr("This creates a new Qt Designer form file."));
    m_formWizard = new FormWizard(wizardParameters, core, this);
    addObject(m_formWizard);

#ifdef CPP_ENABLED
    wizardParameters.setKind(Core::IWizard::ClassWizard);
    wizardParameters.setName(tr("Qt Designer Form Class"));
    wizardParameters.setDescription(tr("This creates a new Qt Designer form class."));
    m_formClassWizard = new FormClassWizard(wizardParameters, core, this);
    addObject(m_formClassWizard);
#endif
    return true;
}

Q_EXPORT_PLUGIN(FormEditorPlugin)
