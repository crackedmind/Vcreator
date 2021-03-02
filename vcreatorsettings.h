#pragma once

#include <coreplugin/dialogs/ioptionspage.h>
#include <texteditor/simplecodestylepreferences.h>
#include <utils/fileutils.h>

namespace VCreator {
namespace Internal {

class VlangSettings : public QObject
{
    Q_OBJECT
public:
    explicit VlangSettings(QObject *parent = nullptr);
    ~VlangSettings();

    static TextEditor::SimpleCodeStylePreferences *globalCodeStyle();
};

class VlangSettingsPage final: public Core::IOptionsPage {
public:
    VlangSettingsPage();

    QWidget *widget() override;
    void apply() override;
    void finish() override;

private:
    std::unique_ptr<QWidget> m_widget;
};

class VlangCodeStyleSettingsPage final: public Core::IOptionsPage {
public:
    VlangCodeStyleSettingsPage();
};

} // namespace Internal
} // namespace Vcreator
