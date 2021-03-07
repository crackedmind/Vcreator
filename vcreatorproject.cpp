#include "vcreatorproject.h"
#include "vcreatorconstants.h"

#include <projectexplorer/buildsystem.h>
#include <projectexplorer/buildtargetinfo.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectnodes.h>
#include <projectexplorer/target.h>

#include <coreplugin/icontext.h>

#include <utils/fileutils.h>
#include <utils/mimetypes/mimetype.h>

#include <QRegularExpression>

using namespace Core;

namespace VCreator {
namespace Internal {

struct ManifestParser {
    QString name;

    void parse(const Utils::FilePath& manifestFile) {
        QRegularExpression re(R"del(name: \'(\w+)\')del");
        QFile file(manifestFile.toString());
        if (file.open(QFile::ReadOnly)) {
            QTextStream stream(&file);

            while (true) {
                const QString line = stream.readLine();
                if (line.isNull())
                    break;

                QRegularExpressionMatch match = re.match(line);
                if(match.hasMatch()) {
                    name = match.captured(1);
                }
            }
        }
    }
};

class ProjectNode : public ProjectExplorer::ProjectNode
{
public:
    ProjectNode(const Utils::FilePath &path)
        : ProjectExplorer::ProjectNode(path)
    {
        setDisplayName(path.toFileInfo().completeBaseName());
        setAddFileFilter("*.v");
    }
};

class FileNode : public ProjectExplorer::FileNode
{
public:
    FileNode(const Utils::FilePath &filePath)
        : ProjectExplorer::FileNode(filePath, ProjectExplorer::FileType::Source)
        , m_displayName(filePath.fileName())
    {
    }

    QString displayName() const override { return m_displayName; }
private:
    QString m_displayName;
};


class BuildSystem : public ProjectExplorer::BuildSystem
{
public:
    explicit BuildSystem(ProjectExplorer::Target *target) : ProjectExplorer::BuildSystem(target)
    {
        // TreeScanner:
        m_treeScanner.setFilter([this](const Utils::MimeType &mimeType, const Utils::FilePath &fn) {
            // Mime checks requires more resources, so keep it last in check list
            auto isIgnored = fn.toString().startsWith(projectFilePath().toString() + ".user")
                             || ProjectExplorer::TreeScanner::isWellKnownBinary(mimeType, fn)
                    || (mimeType.name() != Constants::C_VLANG_MIMETYPE && mimeType.name() != Constants::C_VLANG_PROJECT_MIMETYPE);

            // Cache mime check result for speed up
            if (!isIgnored) {
                auto it = m_mimeBinaryCache.find(mimeType.name());
                if (it != m_mimeBinaryCache.end()) {
                    isIgnored = *it;
                } else {
                    isIgnored = ProjectExplorer::TreeScanner::isMimeBinary(mimeType, fn);
                    m_mimeBinaryCache[mimeType.name()] = isIgnored;
                }
            }

            return isIgnored;
        });
        connect(&m_treeScanner, &ProjectExplorer::TreeScanner::finished, this, [this, target] {
            auto root = std::make_unique<ProjectExplorer::ProjectNode>(projectDirectory());
            root->setDisplayName(target->project()->displayName());
            std::vector<std::unique_ptr<ProjectExplorer::FileNode>> nodePtrs
                = Utils::transform<std::vector>(m_treeScanner.release(), [](ProjectExplorer::FileNode *fn) {
                      return std::unique_ptr<ProjectExplorer::FileNode>(fn);
                  });
            root->addNestedNodes(std::move(nodePtrs));
            setRootProjectNode(std::move(root));

            m_parseGuard.markAsSuccess();
            m_parseGuard = {};

            emitBuildSystemUpdated();
        });

        connect(target->project(),
                &ProjectExplorer::Project::projectFileIsDirty,
                this,
                &BuildSystem::requestDelayedParse);

        requestDelayedParse();
    }
    bool supportsAction(ProjectExplorer::Node *context, ProjectExplorer::ProjectAction action, const ProjectExplorer::Node *node) const override {
        if (node->asFileNode()) {
            return action == ProjectExplorer::ProjectAction::Rename
                || action == ProjectExplorer::ProjectAction::EraseFile;
        }
        if (node->isFolderNodeType() || node->isProjectNodeType()) {
            return action == ProjectExplorer::ProjectAction::AddNewFile
                || action == ProjectExplorer::ProjectAction::EraseFile
                || action == ProjectExplorer::ProjectAction::AddExistingFile;
        }
        return BuildSystem::supportsAction(context, action, node);
    }

    bool addFiles(ProjectExplorer::Node *, const QStringList &, QStringList *) override {
        requestDelayedParse();
        return true;
    }

    ProjectExplorer::RemovedFilesFromProject removeFiles(ProjectExplorer::Node *, const QStringList &, QStringList *) override {
        requestDelayedParse();
        return ProjectExplorer::RemovedFilesFromProject::Ok;
    }


    bool deleteFiles(ProjectExplorer::Node *, const QStringList &) override {
        requestDelayedParse();
        return true;
    }

    bool renameFile(ProjectExplorer::Node *, const QString &, const QString &) override {
        requestDelayedParse();
        return true;
    }

    void triggerParsing() final {
        m_parseGuard = guardParsingRun();
        m_treeScanner.asyncScanForFiles(target()->project()->projectDirectory());
    }

private:
    QHash<QString, bool> m_mimeBinaryCache;
    ProjectExplorer::TreeScanner m_treeScanner;
    ParseGuard m_parseGuard;
};

VlangProject::VlangProject(const Utils::FilePath &filename)
    : ProjectExplorer::Project(Constants::C_VLANG_PROJECT_MIMETYPE, filename)
{
    setId(Constants::C_VLANG_PROJECT_ID);
    ManifestParser manifest;
    manifest.parse(filename);
    setDisplayName(manifest.name);

    setNeedsBuildConfigurations(false);

    setBuildSystemCreator([](ProjectExplorer::Target *t) { return new BuildSystem(t); });
}

ProjectExplorer::Project::RestoreResult VlangProject::fromMap(const QVariantMap &map, QString *errorMessage)
{
    RestoreResult res = ProjectExplorer::Project::fromMap(map, errorMessage);
    if (res == RestoreResult::Ok)
        addTargetForDefaultKit();

    return res;
}

} // namespace Internal
} // namespace Vcreator
