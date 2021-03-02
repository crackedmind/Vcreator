#pragma once

#include "vcreatorlexer.h"

#include <texteditor/textdocumentlayout.h>
#include <texteditor/syntaxhighlighter.h>

namespace VCreator {
namespace Internal {

class VlangHighlighter : public TextEditor::SyntaxHighlighter {
    Q_OBJECT

public:
    VlangHighlighter();

protected:
    void highlightBlock(const QString &text) override;

    int onBlockStart();
    void onBlockEnd(int state);

    // The functions are notified whenever parentheses are encountered.
    // Custom behaviour can be added, for example storing info for indenting.
    void onOpeningParenthesis(QChar parenthesis, int pos, bool atStart);
    void onClosingParenthesis(QChar parenthesis, int pos, bool atEnd);

//    bool maybeQmlKeyword(const QStringView &text) const;
//    bool maybeQmlBuiltinType(const QStringView &text) const;

private:
//    bool m_qmlEnabled;
    int m_braceDepth;
    int m_foldingIndent;
    bool m_inMultilineComment;

    Scanner m_scanner;
    TextEditor::Parentheses m_currentBlockParentheses;
};

} // namespace Internal
} // namespace Vcreator
