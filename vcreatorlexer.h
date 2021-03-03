#pragma once

#include <QList>
#include <QString>

namespace VCreator {
namespace Internal {

class Token
{
public:
    enum Kind {
        EndOfFile,
        Keyword,
        Identifier,
        String,
        Comment,
        Number,
        Hash,
        Operator,
        BuiltinType,
        BuiltinFn,
        Function,
        LeftParenthesis,
        RightParenthesis,
        LeftBrace,
        RightBrace,
        LeftBracket,
        RightBracket,
        Semicolon,
        Colon,
        Comma,
        Dot,
        Delimiter,
        RegExp
    };

    inline Token(): offset(0), length(0), kind(EndOfFile) {}
    inline Token(int o, int l, Kind k): offset(o), length(l), kind(k) {}
    inline int begin() const { return offset; }
    inline int end() const { return offset + length; }
    inline bool is(int k) const { return k == kind; }
    inline bool isNot(int k) const { return k != kind; }

public:
    int offset;
    int length;
    Kind kind;
};

class Scanner {
public:
    enum {
        Normal = 0,
        MultiLineComment = 1,
        MultiLineStringDQuote = 2,
        MultiLineStringSQuote = 3,
        MultiLineMask = 3,
    };

    Scanner() : _state(Normal), _scanComments(true) {
    }

    virtual ~Scanner() = default;

    bool scanComments() const;
    void setScanComments(bool scanComments);

    QList<Token> operator()(const QString &text, int startState = Normal);
    int state() const { return _state; }

    bool isKeyword(const QString &text) const;

private:
    int _state;
    bool _scanComments: 1;
};

} // namespace Internal
} // namespace Vcreator
