#pragma once

#include <texteditor/texteditor.h>

namespace VCreator {
namespace Internal {

class EditorFactory : public TextEditor::TextEditorFactory
{
public:
    EditorFactory();

    static void decorateEditor(TextEditor::TextEditorWidget *editor);
};

} // namespace Internal
} // namespace Vcreator

