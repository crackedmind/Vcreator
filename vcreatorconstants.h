#pragma once

namespace VCreator {
namespace Constants {

const char C_VLANG_PROJECT_ID[] = "Vcreator.VlangProject";
const char C_VLANG_EDITOR_ID[] = "Vcreator.VlangEditor";

const char C_EDITOR_DISPLAY_NAME[] =
        QT_TRANSLATE_NOOP("OpenWith::Editors", "Vlang Editor");

const char C_VLANGUAGE_ID[] = "V";
const char C_VLANGCODESTYLESETTINGSPAGE_ID[] = "Vcreator.VlangCodeStyleSettings";
const char C_VLANGCODESTYLESETTINGSPAGE_DISPLAY[] = QT_TRANSLATE_NOOP("VlangCodeStyleSettingsPage", "Code Style");
const char C_VLANGCODESTYLESETTINGSPAGE_CATEGORY[] = "Z.V";
const char C_VLANGCODESTYLESETTINGSPAGE_CATEGORY_DISPLAY[] = QT_TRANSLATE_NOOP("VlangCodeStyleSettingsPage", "V");

const char C_VLANGSETTINGSPAGE_ID[] = "Vcreator.VlangSettings";
const char C_VLANGSETTINGSPAGE_DISPLAY[] = QT_TRANSLATE_NOOP("VcreatorSettingsPage", "Tools");
const char C_VLANGSETTINGSPAGE_CATEGORY[] = "Z.V";
const char C_VLANGSETTINGSPAGE_CATEGORY_DISPLAY[] = QT_TRANSLATE_NOOP("VcreatorSettingsPage", "Tools");

const char C_VLANGUAGE_NAME[] = QT_TRANSLATE_NOOP("VcreatorCodeStylePreferencesFactory", "V");
const char C_VLANGGLOBALCODESTYLE_ID[] = "VlangGlobal";
const char C_VLANGSNIPPETSGROUP_ID[] = "Vlang.VlangSnippetsGroup";

const char C_VCODESTYLEPREVIEWSNIPPET[] =
        R"code(module main

import os

struct Foo {
    member int
}

fn main() {
    foo := Foo{member: 1}
    println('Hello, World!')
}
)code";

const char C_VLANG_MIMETYPE[] = "application/x-vlang";
//const char C_VLANG_MIME_ICON[] = "text-x-vlang";
const char C_VLANG_PROJECT_MIMETYPE[] = "text/x-vlang-project";

const char C_VLANG_SETTINGS_GROUP[] = "V";

} // namespace Constants
} // namespace Vcreator
