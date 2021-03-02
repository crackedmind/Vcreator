#include "vcreatoreditor.h"
#include "vcreatorhighlighter.h"
#include "vcreatorconstants.h"
#include "vcreatorindenter.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/commandbutton.h>

#include <texteditor/textdocument.h>
#include <texteditor/texteditoractionhandler.h>

namespace VCreator {
namespace Internal {

EditorFactory::EditorFactory()
{
    addMimeType("application/x-vlang");
    setId(Constants::C_VLANG_EDITOR_ID);
    setDisplayName(
        QCoreApplication::translate("OpenWith::Editors", "Vlang Editor"));

    setEditorActionHandlers(TextEditor::TextEditorActionHandler::Format
                            | TextEditor::TextEditorActionHandler::UnCommentSelection
                            | TextEditor::TextEditorActionHandler::UnCollapseAll);

    setDocumentCreator([] {
        auto td = new TextEditor::TextDocument(Constants::C_VLANG_EDITOR_ID);
        td->setMimeType("application/x-vlang");
        return td;
    });
    setEditorWidgetCreator([]{
        auto tw = new TextEditor::TextEditorWidget;
        tw->setLanguageSettingsId(Constants::C_VLANGUAGE_ID);
        return tw;
    });
    setIndenterCreator([](QTextDocument *doc) {
        return new VlangIndenter(doc);
    });
    setSyntaxHighlighterCreator([] { return new VlangHighlighter(); });
    setCommentDefinition(Utils::CommentDefinition::CppStyle);
    setUseGenericHighlighter(false);
    setParenthesesMatchingEnabled(true);
    setCodeFoldingSupported(true);
}

void EditorFactory::decorateEditor(TextEditor::TextEditorWidget *editor)
{
    editor->textDocument()->setSyntaxHighlighter(new VlangHighlighter());
    editor->textDocument()->setIndenter(new VlangIndenter(editor->textDocument()->document()));
}


} // namespace Internal
} // namespace Vcreator
