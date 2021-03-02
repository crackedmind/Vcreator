#include "vcreatorindenter.h"

#include "vcreatorlexer.h"

#include <texteditor/icodestylepreferences.h>
#include <texteditor/tabsettings.h>
#include <texteditor/simplecodestylepreferences.h>
#include <texteditor/tabsettings.h>
namespace VCreator {
namespace Internal {

VlangIndenter::VlangIndenter(QTextDocument *doc) : TextEditor::TextIndenter(doc)
{

}

bool VlangIndenter::isElectricCharacter(const QChar &ch) const
{
    return ch == '{';
}

void VlangIndenter::indentBlock(const QTextBlock &block, const QChar &typedChar, const TextEditor::TabSettings &settings, int cursorPositionInEditor)
{
    const QString currentLine = block.text();

    const QTextBlock previousBlock = block.previous();
    const QString previousLine = previousBlock.text();
    const int previousState = previousBlock.userState();

    if (!previousBlock.isValid()) {
        settings.indentLine(block, 0);
        return;
    }

    // Calculate indentation
    int indentation = 0;
    if (rightTrimmed(currentLine).isEmpty()) {
        // Current line is empty so we calculate indentation based on previous line
        const int indentationDiff = calculateIndentationDiff(previousLine, previousState, settings.m_indentSize);
        indentation = settings.indentationColumn(previousLine) + indentationDiff;
    }
    else {
        // We don't change indentation if the line is already indented.
        // This is safer but sub optimal
        indentation = settings.indentationColumn(block.text());
    }

    // Sets indentation
    settings.indentLine(block, std::max(0, indentation));
}

bool VlangIndenter::startsBlock(const QString &line, int state) const
{
    return false;
}

bool VlangIndenter::endsBlock(const QString &line, int state) const
{
    return false;
}

int VlangIndenter::calculateIndentationDiff(const QString &previousLine, int previousState, int indentSize) const
{
    return 0;
}

QString VlangIndenter::rightTrimmed(const QString &str)
{
    int n = str.size() - 1;
    for (; n >= 0; --n) {
        if (!str.at(n).isSpace())
            return str.left(n + 1);
    }
    return QString();
}


} // namespace Internal
} // namespace Vcreator
