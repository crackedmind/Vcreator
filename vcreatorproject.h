#pragma once

#include <projectexplorer/project.h>

namespace VCreator {
namespace Internal {

class VlangProject : public ProjectExplorer::Project
{
    Q_OBJECT
public:
    explicit VlangProject(const Utils::FilePath &filename);

    bool needsConfiguration() const final { return false; }
private:
    RestoreResult fromMap(const QVariantMap &map, QString *errorMessage) override;
};

} // namespace Internal
} // namespace Vcreator
