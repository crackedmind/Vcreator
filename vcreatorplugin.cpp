#include "vcreatorplugin.h"
#include "vcreatorconstants.h"
#include "vcreatorproject.h"
#include "vcreatorsettings.h"
#include "vcreatoreditor.h"
#include "vcreatorhighlighter.h"
#include "vcreatorindenter.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <coreplugin/fileiconprovider.h>
#include <projectexplorer/projectmanager.h>
#include <texteditor/snippets/snippetprovider.h>
#include <texteditor/texteditor.h>
#include <texteditor/textdocument.h>

#include <utils/theme/theme.h>

namespace VCreator {
namespace Internal {

struct PluginPrivate {
    VlangSettings settings;
    EditorFactory editorFactory;
    VlangCodeStyleSettingsPage codeStylePage;
};


Plugin::Plugin()
{
    // Create your members
}

Plugin::~Plugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
    delete d;
}

bool Plugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    d = new PluginPrivate;

    TextEditor::SnippetProvider::registerGroup(Constants::C_VLANGSNIPPETSGROUP_ID,
                                               tr("V", "SnippetProvider"),
                                               &EditorFactory::decorateEditor);

    ProjectExplorer::ProjectManager::registerProjectType<VlangProject>(Constants::C_VLANG_PROJECT_MIMETYPE);

    return true;
}

void Plugin::extensionsInitialized()
{
    Core::FileIconProvider::registerIconOverlayForMimeType(":/v/images/settingscategory_v.png", Constants::C_VLANG_MIMETYPE);
}

ExtensionSystem::IPlugin::ShutdownFlag Plugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

} // namespace Internal
} // namespace Vcreator
