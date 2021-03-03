#include "vcreatorhighlighter.h"
#include "vcreatorlexer.h"

#include <texteditor/textdocument.h>
#include <texteditor/texteditorconstants.h>
#include <utils/porting.h>

#include <QRegularExpression>

namespace VCreator {
namespace Internal {

VlangHighlighter::VlangHighlighter()
    :
      m_braceDepth(0),
      m_foldingIndent(0),
      m_inMultilineComment(false)
{
    m_currentBlockParentheses.reserve(20);
    setDefaultTextFormatCategories();
}

void VlangHighlighter::highlightBlock(const QString &text)
{
    const QList<Token> tokens = m_scanner(text, onBlockStart());

    int index = 0;
    while (index < tokens.size()) {
        const Token &token = tokens.at(index);

        switch (token.kind) {

            case Token::Keyword:
                setFormat(token.offset, token.length, formatForCategory(TextEditor::C_KEYWORD));
                break;

            case Token::String: {
                QRegularExpression re(R"del((\$([\w.]+|\{.*?\})))del");
                QString t = text.mid(token.offset, token.length);
                auto it = re.globalMatch(t);
                int matches = 0;
                setFormat(token.offset, token.length, formatForCategory(TextEditor::C_STRING));
                while(it.hasNext()) {
                    auto match = it.next();
                    setFormat(token.offset + match.capturedStart(1), match.capturedEnd(1) - match.capturedStart(1), formatForCategory(TextEditor::C_TYPE));
                    matches++;
                }
            }
                break;

            case Token::Number:
                setFormat(token.offset, token.length, formatForCategory(TextEditor::C_NUMBER));
                break;

            case Token::Hash: {
                static QSet<QString> hashDecls = {"#include", "#flag", "#pkgconfig", "#define"};
                if (hashDecls.contains(text.mid(token.offset, token.length)))
                    setFormat(token.offset, token.length, formatForCategory(TextEditor::C_PREPROCESSOR));
                else
                    setFormat(token.offset, token.length, formatForCategory(TextEditor::C_TEXT));
            } break;

            case Token::Comment:
                if (m_inMultilineComment
                    && Utils::midView(text, token.end() - 2, 2) == QLatin1String("*/")) {
                    onClosingParenthesis(QLatin1Char('-'), token.end() - 1, index == tokens.size()-1);
                    m_inMultilineComment = false;
                } else if (!m_inMultilineComment
                           && (m_scanner.state() & Scanner::MultiLineMask) == Scanner::MultiLineComment
                           && index == tokens.size() - 1) {
                    onOpeningParenthesis(QLatin1Char('+'), token.offset, index == 0);
                    m_inMultilineComment = true;
                }
                setFormat(token.offset, token.length, formatForCategory(TextEditor::C_COMMENT));
                break;

            case Token::BuiltinType:
                setFormat(token.offset, token.length, formatForCategory(TextEditor::C_PRIMITIVE_TYPE));
                break;

            case Token::BuiltinFn:
            case Token::Function:
                setFormat(token.offset, token.length, formatForCategory(TextEditor::C_FUNCTION));
                break;

            case Token::LeftParenthesis:
                onOpeningParenthesis(QLatin1Char('('), token.offset, index == 0);
                break;

            case Token::RightParenthesis:
                onClosingParenthesis(QLatin1Char(')'), token.offset, index == tokens.size()-1);
                break;

            case Token::LeftBrace:
                onOpeningParenthesis(QLatin1Char('{'), token.offset, index == 0);
                break;

            case Token::RightBrace:
                onClosingParenthesis(QLatin1Char('}'), token.offset, index == tokens.size()-1);
                break;

            case Token::LeftBracket:
                onOpeningParenthesis(QLatin1Char('['), token.offset, index == 0);
                break;

            case Token::RightBracket:
                onClosingParenthesis(QLatin1Char(']'), token.offset, index == tokens.size()-1);
                break;

            case Token::Identifier: {
                setFormat(token.offset, token.length, formatForCategory(TextEditor::C_TEXT));
            }   break;

            case Token::Delimiter:
                break;

            default:
                break;
        } // end swtich

        ++index;
    }

    int previousVlangTokenEnd = 0;
    for (const auto &VlangToken : tokens) {
        setFormat(previousVlangTokenEnd, VlangToken.begin() - previousVlangTokenEnd, formatForCategory(TextEditor::C_VISUAL_WHITESPACE));

        switch (VlangToken.kind) {
        case Token::Comment:
        case Token::String: {
            int i = VlangToken.begin(), e = VlangToken.end();
            while (i < e) {
                const QChar ch = text.at(i);
                if (ch.isSpace()) {
                    const int start = i;
                    do {
                        ++i;
                    } while (i < e && text.at(i).isSpace());
                    setFormat(start, i - start, formatForCategory(TextEditor::C_VISUAL_WHITESPACE));
                } else {
                    ++i;
                }
            }
        } break;

        default:
            break;
        } // end of switch

        previousVlangTokenEnd = VlangToken.end();
    }

    setFormat(previousVlangTokenEnd, text.length() - previousVlangTokenEnd, formatForCategory(TextEditor::C_VISUAL_WHITESPACE));

    setCurrentBlockState(m_scanner.state());
    onBlockEnd(m_scanner.state());
}

int VlangHighlighter::onBlockStart()
{
    m_currentBlockParentheses.clear();
    m_braceDepth = 0;
    m_foldingIndent = 0;
    m_inMultilineComment = false;
    if (TextEditor::TextBlockUserData *userData = TextEditor::TextDocumentLayout::textUserData(currentBlock())) {
        userData->setFoldingIndent(0);
        userData->setFoldingStartIncluded(false);
        userData->setFoldingEndIncluded(false);
    }

    int state = 0;
    int previousState = previousBlockState();
    if (previousState != -1) {
        state = previousState & 0xff;
        m_braceDepth = (previousState >> 8);
        m_inMultilineComment = ((state & Scanner::MultiLineMask) == Scanner::MultiLineComment);
    }
    m_foldingIndent = m_braceDepth;

    return state;
}

void VlangHighlighter::onBlockEnd(int state)
{
    setCurrentBlockState((m_braceDepth << 8) | state);
    TextEditor::TextDocumentLayout::setParentheses(currentBlock(), m_currentBlockParentheses);
    TextEditor::TextDocumentLayout::setFoldingIndent(currentBlock(), m_foldingIndent);
}

void VlangHighlighter::onOpeningParenthesis(QChar parenthesis, int pos, bool atStart)
{
    if (parenthesis == QLatin1Char('{') || parenthesis == QLatin1Char('[') || parenthesis == QLatin1Char('(')) {
        ++m_braceDepth;
        // if a folding block opens at the beginning of a line, treat the entire line
        // as if it were inside the folding block
        if (atStart)
            TextEditor::TextDocumentLayout::userData(currentBlock())->setFoldingStartIncluded(true);
    }
    m_currentBlockParentheses.push_back(TextEditor::Parenthesis(TextEditor::Parenthesis::Opened, parenthesis, pos));
}

void VlangHighlighter::onClosingParenthesis(QChar parenthesis, int pos, bool atEnd)
{
    if (parenthesis == QLatin1Char('}') || parenthesis == QLatin1Char(']') || parenthesis == QLatin1Char(')')) {
        --m_braceDepth;
        if (atEnd)
            TextEditor::TextDocumentLayout::userData(currentBlock())->setFoldingEndIncluded(true);
        else
            m_foldingIndent = qMin(m_braceDepth, m_foldingIndent); // folding indent is the minimum brace depth of a block
    }
    m_currentBlockParentheses.push_back(TextEditor::Parenthesis(TextEditor::Parenthesis::Closed, parenthesis, pos));
}


} // namespace Internal
} // namespace Vcreator
