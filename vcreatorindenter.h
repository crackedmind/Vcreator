#pragma once

#include <texteditor/textindenter.h>

namespace VCreator {
namespace Internal {

class VlangIndenter : public TextEditor::TextIndenter
{
public:
    explicit VlangIndenter(QTextDocument *doc);

    bool isElectricCharacter(const QChar &ch) const override;

    void indentBlock(const QTextBlock &block,
                     const QChar &typedChar,
                     const TextEditor::TabSettings &settings,
                     int cursorPositionInEditor = -1) override;

private:
    bool startsBlock(const QString &line, int state) const;
    bool endsBlock(const QString &line, int state) const;

    int calculateIndentationDiff(const QString &previousLine,
                                 int previousState,
                                 int indentSize) const;

    static QString rightTrimmed(const QString &str);
};

} // namespace Internal
} // namespace Vcreator
