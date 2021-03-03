#include "vcreatorlexer.h"

#include "QRegularExpression"
#include <QSet>

namespace VCreator {
namespace Internal {

static const QSet<QString> vKeywords = {
    QLatin1String("assert"),
    QLatin1String("struct"),
    QLatin1String("if"),
    QLatin1String("else"),
    QLatin1String("asm"),
    QLatin1String("return"),
    QLatin1String("module"),
    QLatin1String("sizeof"),
    QLatin1String("_likely_"),
    QLatin1String("_unlikely_"),
    QLatin1String("go"),
    QLatin1String("goto"),
    QLatin1String("const"),
    QLatin1String("mut"),
    QLatin1String("shared"),
    QLatin1String("lock"),
    QLatin1String("rlock"),
    QLatin1String("type"),
    QLatin1String("for"),
    QLatin1String("fn"),
    QLatin1String("true"),
    QLatin1String("false"),
    QLatin1String("continue"),
    QLatin1String("break"),
    QLatin1String("import"),
    QLatin1String("unsafe"),
    QLatin1String("typeof"),
    QLatin1String("enum"),
    QLatin1String("interface"),
    QLatin1String("pub"),
    QLatin1String("in"),
    QLatin1String("atomic"),
    QLatin1String("or"),
    QLatin1String("__global"),
    QLatin1String("union"),
    QLatin1String("static"),
    QLatin1String("as"),
    QLatin1String("defer"),
    QLatin1String("match"),
    QLatin1String("select"),
    QLatin1String("none"),
    QLatin1String("__offsetof"),
    QLatin1String("is"),
    QLatin1String("$for"),
    QLatin1String("$if"),
    QLatin1String("$else"),
    QLatin1String("$embed_file"),
    QLatin1String("$tmpl"),
    QLatin1String("$env")
};

static QSet<QString> vBuiltinTypes {
    "void",
    "voidptr",
    "byteptr",
    "charptr",
    "i8",
    "i16",
    "int",
    "i64",
    "byte",
    "u16",
    "u32",
    "u64",
    "f32",
    "f64",
    "char",
    "bool",
    "none",
    "string",
    "ustring",
    "rune",
    "array",
    "map",
    "chan",
    "size_t",
    "any"
};

static QSet<QString> vBuiltinFn {
    "exit",
    "panic",
    "eprintln",
    "eprint",
    "print",
    "println",
    "malloc",
    "v_realloc",
    "v_calloc",
    "vcalloc",
    "free",
    "memdup",
    "is_atty",
    "isnil",
    "print_backtrace"
};

static QSet<QString> vOperators {
    "*", "/", "%", "<<", ">>", "&",
    "+", "-", "|", "^"
    "==", "!=", "<", "<=", ">", ">=",
    "&&", "||",
    "+=", "-=", "*=", "/=", "%=",
    "&=", "|=", "^=",
    ">>=", "<<="
};

static bool isNumberChar(QChar ch)
{
    return ch.isLetterOrNumber();
}

static bool isIdentifierChar(QChar ch) {
    switch (ch.unicode()) {
    case '$': case '_':
        return true;

    default:
        return ch.isLetterOrNumber();
    }
};

static inline int multiLineState(int state)
{
    return state & Scanner::MultiLineMask;
}

static inline void setMultiLineState(int *state, int s)
{
    *state = s | (*state & ~Scanner::MultiLineMask);
}

QList<Token> Scanner::operator()(const QString &text, int startState)
{
    _state = startState;
    QList<Token> tokens;

    int index = 0;

    if (multiLineState(_state) == MultiLineComment) {
        int start = -1;
        while (index < text.length()) {
            const QChar ch = text.at(index);

            if (start == -1 && !ch.isSpace())
                start = index;

            QChar la;
            if (index + 1 < text.length())
                la = text.at(index + 1);

            if (ch == QLatin1Char('*') && la == QLatin1Char('/')) {
                setMultiLineState(&_state, Normal);
                index += 2;
                break;
            } else {
                ++index;
            }
        }

        if (_scanComments && start != -1)
            tokens.append(Token(start, index - start, Token::Comment));
    } else if (multiLineState(_state) == MultiLineStringDQuote || multiLineState(_state) == MultiLineStringSQuote) {
        const QChar quote = (_state == MultiLineStringDQuote ? QLatin1Char('"') : QLatin1Char('\''));
        const int start = index;
        while (index < text.length()) {
            const QChar ch = text.at(index);

            if (ch == quote)
                break;
            else if (index + 1 < text.length() && ch == QLatin1Char('\\'))
                index += 2;
            else
                ++index;
        }
        if (index < text.length()) {
            ++index;
            setMultiLineState(&_state, Normal);
        }
        if (start < index)
            tokens.append(Token(start, index - start, Token::String));
    }

    while (index < text.length()) {
        const QChar ch = text.at(index);

        QChar la; // lookahead char
        if (index + 1 < text.length())
            la = text.at(index + 1);

        switch (ch.toLatin1()) {
        case '/':
            if (la == QLatin1Char('/')) {
                if (_scanComments)
                    tokens.append(Token(index, text.length() - index, Token::Comment));
                index = text.length();
            } else if (la == QLatin1Char('*')) {
                const int start = index;
                index += 2;
                setMultiLineState(&_state, MultiLineComment);
                while (index < text.length()) {
                    const QChar ch = text.at(index);
                    QChar la;
                    if (index + 1 < text.length())
                        la = text.at(index + 1);

                    if (ch == QLatin1Char('*') && la == QLatin1Char('/')) {
                        setMultiLineState(&_state, Normal);
                        index += 2;
                        break;
                    } else {
                        ++index;
                    }
                }
                if (_scanComments)
                    tokens.append(Token(start, index - start, Token::Comment));
            } else
                tokens.append(Token(index++, 1, Token::Delimiter));
            break;

        case '\'':
        case '"': {
            const QChar quote = ch;
            const int start = index;
            ++index;
            while (index < text.length()) {
                const QChar ch = text.at(index);

                if (ch == quote)
                    break;
                else if (index + 1 < text.length() && ch == QLatin1Char('\\'))
                    index += 2;
                else
                    ++index;
            }

            if (index < text.length()) {
                ++index;
                // good one
            } else {
                if (quote == '"')
                    setMultiLineState(&_state, MultiLineStringDQuote);
                else
                    setMultiLineState(&_state, MultiLineStringSQuote);
            }

            tokens.append(Token(start, index - start, Token::String));
        } break;

        case '.':
            tokens.append(Token(index++, 1, Token::Dot));
            break;

         case '(':
            tokens.append(Token(index++, 1, Token::LeftParenthesis));
            break;

         case ')':
            tokens.append(Token(index++, 1, Token::RightParenthesis));
            break;

         case '[':
            tokens.append(Token(index++, 1, Token::LeftBracket));
            break;

         case ']':
            tokens.append(Token(index++, 1, Token::RightBracket));
            break;

         case '{':
            tokens.append(Token(index++, 1, Token::LeftBrace));
            break;

         case '}':
            tokens.append(Token(index++, 1, Token::RightBrace));
            break;

         case ';':
            tokens.append(Token(index++, 1, Token::Semicolon));
            break;

         case ':':
            tokens.append(Token(index++, 1, Token::Colon));
            break;

         case ',':
            tokens.append(Token(index++, 1, Token::Comma));
            break;
         case '#': {
             const int start = index;
             do {
                 ++index;
             } while (index < text.length() && isIdentifierChar(text.at(index)));
             tokens.append(Token(start, index - start, Token::Hash));
         } break;

        default:
            if (ch.isSpace()) {
                do {
                    ++index;
                } while (index < text.length() && text.at(index).isSpace());
            } else if (ch.isNumber()) {
                const int start = index;
                do {
                    ++index;
                } while (index < text.length() && isNumberChar(text.at(index)));
                tokens.append(Token(start, index - start, Token::Number));
            } else if (ch.isLetter() || ch == '_' || ch == QLatin1Char('$')) {
                const int start = index;
                do {
                    ++index;
                } while (index < text.length() && isIdentifierChar(text.at(index)));

                auto var = text.mid(start, index - start);

                if (vKeywords.contains(var))
                    tokens.append(Token(start, index - start, Token::Keyword));
                else if(vBuiltinTypes.contains(var))
                    tokens.append(Token(start, index - start, Token::BuiltinType));
                else if(vBuiltinFn.contains(var))
                    tokens.append(Token(start, index - start, Token::Function));
                else
                    tokens.append(Token(start, index - start, Token::Identifier));
            } else {
                tokens.append(Token(index++, 1, Token::Delimiter));
            }
        } // end of switch
    }

    return tokens;
}

//bool VlangScanner::isKeyword(const QString &text) const
//{
//    return keywords.contains(text);
//}



} // namespace Internal
} // namespace Vcreator
