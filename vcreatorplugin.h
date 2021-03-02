#pragma once

#include "vcreator_global.h"

#include <extensionsystem/iplugin.h>

namespace VCreator {
namespace Internal {

struct PluginPrivate;

class Plugin final: public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Vcreator.json")

public:
    Plugin();
    ~Plugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    PluginPrivate *d = nullptr;
};

} // namespace Internal
} // namespace Vcreator
