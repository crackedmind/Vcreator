#include "vcreatorsettings.h"
#include "vcreatorconstants.h"
#include "vcreatorindenter.h"
#include "vcreatorhighlighter.h"

#include <texteditor/simplecodestylepreferences.h>
#include <texteditor/simplecodestylepreferenceswidget.h>
#include <texteditor/snippets/snippeteditor.h>
#include <texteditor/codestyleeditor.h>
#include <texteditor/texteditorsettings.h>
#include <texteditor/tabsettings.h>
#include <texteditor/textdocument.h>
#include <texteditor/icodestylepreferencesfactory.h>
#include <texteditor/texteditorsettings.h>
#include <texteditor/codestylepool.h>
#include <texteditor/displaysettings.h>

#include <coreplugin/icore.h>

#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFormLayout>
#include <utils/pathchooser.h>

using namespace TextEditor;

namespace VCreator {
namespace Internal {

static SimpleCodeStylePreferences *m_globalCodeStyle = nullptr;

class SettingsWidget final : public QWidget {
public:
    explicit SettingsWidget(QWidget *parent = nullptr);

private:

    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_2;
    QFormLayout *formLayout;
    QLabel *pathLabel;
    Utils::PathChooser *pathWidget;
    QSpacerItem *verticalSpacer;
};

class rCodeStylePreferencesWidget : public QWidget
{
    QGridLayout *gridLayout;
    TextEditor::SimpleCodeStylePreferencesWidget *tabPreferencesWidget;
    TextEditor::SnippetEditorWidget *previewTextEdit;
    QSpacerItem *verticalSpacer;
public:
    rCodeStylePreferencesWidget(TextEditor::ICodeStylePreferences *preferences, QWidget *parent = nullptr);
    ~rCodeStylePreferencesWidget() = default;

private:
    void decorateEditor(const TextEditor::FontSettings &fontSettings);

    void setVisualizeWhitespace(bool on);

    void updatePreview();

    TextEditor::ICodeStylePreferences *m_preferences;
};

class CodeStyleSettingsWidget : public Core::IOptionsPageWidget
{
    Q_DECLARE_TR_FUNCTIONS(CodeStyleSettings)

public:
    CodeStyleSettingsWidget();

private:
    void apply() final;
    void finish() final;

    TextEditor::SimpleCodeStylePreferences *m_vlangCodeStylePreferences;
};

class CodeStylePreferencesFactory : public TextEditor::ICodeStylePreferencesFactory
{
    Q_DECLARE_TR_FUNCTIONS(VcreatorCodeStylePreferencesFactory)

public:
    CodeStylePreferencesFactory();

    Utils::Id languageId() override;
    QString displayName() override;
    TextEditor::ICodeStylePreferences *createCodeStyle() const override;
    QWidget *createEditor(TextEditor::ICodeStylePreferences *preferences,
                          QWidget *parent) const override;

    TextEditor::Indenter *createIndenter(QTextDocument *doc) const override;

    QString snippetProviderGroupId() const override;
    QString previewText() const override;
};

VlangSettings::VlangSettings(QObject *parent)
    : QObject(parent)
{
    auto factory = new CodeStylePreferencesFactory();
    TextEditorSettings::registerCodeStyleFactory(factory);

    // code style pool
    auto pool = new CodeStylePool(factory, this);
    TextEditorSettings::registerCodeStylePool(Constants::C_VLANGUAGE_ID, pool);

    m_globalCodeStyle = new SimpleCodeStylePreferences();
    m_globalCodeStyle->setDelegatingPool(pool);
    m_globalCodeStyle->setDisplayName(tr("Global", "Settings"));
    m_globalCodeStyle->setId(Constants::C_VLANGGLOBALCODESTYLE_ID);
    pool->addCodeStyle(m_globalCodeStyle);
    TextEditorSettings::registerCodeStyle(Constants::C_VLANGUAGE_ID, m_globalCodeStyle);

    auto codeStyle = new SimpleCodeStylePreferences();
    codeStyle->setId("vlang");
    codeStyle->setDisplayName(tr("V"));
    codeStyle->setReadOnly(true);

    TabSettings tabSettings;
    tabSettings.m_tabPolicy = TabSettings::TabsOnlyTabPolicy;
    tabSettings.m_tabSize = 4;
    tabSettings.m_indentSize = 4;
    tabSettings.m_continuationAlignBehavior = TabSettings::ContinuationAlignWithIndent;
    codeStyle->setTabSettings(tabSettings);

    pool->addCodeStyle(codeStyle);

    m_globalCodeStyle->setCurrentDelegate(codeStyle);

    pool->loadCustomCodeStyles();

    // load global settings (after built-in settings are added to the pool)
    QSettings *s = Core::ICore::settings();
    m_globalCodeStyle->fromSettings(QLatin1String(Constants::C_VLANGUAGE_ID), s);

    TextEditorSettings::registerMimeTypeForLanguageId(Constants::C_VLANG_MIMETYPE,
                                                      Constants::C_VLANGUAGE_ID);
}

VlangSettings::~VlangSettings()
{
    TextEditorSettings::unregisterCodeStyle(Constants::C_VLANGUAGE_ID);
    TextEditorSettings::unregisterCodeStylePool(Constants::C_VLANGUAGE_ID);
    TextEditorSettings::unregisterCodeStyleFactory(Constants::C_VLANGUAGE_ID);

    delete m_globalCodeStyle;
    m_globalCodeStyle = nullptr;
}

TextEditor::SimpleCodeStylePreferences *VlangSettings::globalCodeStyle()
{
    return m_globalCodeStyle;
}

VlangSettingsPage::VlangSettingsPage()
{
    setId(Constants::C_VLANGSETTINGSPAGE_ID);
    setDisplayName(Constants::C_VLANGSETTINGSPAGE_DISPLAY);
    setCategory(Constants::C_VLANGSETTINGSPAGE_CATEGORY);
    setDisplayCategory("V");
    setCategoryIconPath(":/v/images/settingscategory_v.png");
}

QWidget *VlangSettingsPage::widget()
{
    if (!m_widget)
        m_widget.reset(new SettingsWidget);

    return m_widget.get();
}

void VlangSettingsPage::apply()
{

}

void VlangSettingsPage::finish()
{
    m_widget.reset();
}

VlangCodeStyleSettingsPage::VlangCodeStyleSettingsPage()
{
    setId(Constants::C_VLANGCODESTYLESETTINGSPAGE_ID);
    setDisplayName(tr(Constants::C_VLANGCODESTYLESETTINGSPAGE_DISPLAY));
    setCategory(Constants::C_VLANGCODESTYLESETTINGSPAGE_CATEGORY);
    setDisplayCategory(CodeStyleSettingsWidget::tr("V"));
    setCategoryIconPath(":/v/images/settingscategory_v.png");
    setWidgetCreator([] { return new CodeStyleSettingsWidget; });
}

rCodeStylePreferencesWidget::rCodeStylePreferencesWidget(ICodeStylePreferences *preferences, QWidget *parent)
    : QWidget(parent), m_preferences(preferences) {

    resize(138, 112);
    gridLayout = new QGridLayout(this);
    tabPreferencesWidget = new TextEditor::SimpleCodeStylePreferencesWidget(this);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(tabPreferencesWidget->sizePolicy().hasHeightForWidth());
    tabPreferencesWidget->setSizePolicy(sizePolicy);

    gridLayout->addWidget(tabPreferencesWidget, 0, 0, 1, 1);

    previewTextEdit = new TextEditor::SnippetEditorWidget(this);

    gridLayout->addWidget(previewTextEdit, 0, 1, 2, 1);

    verticalSpacer = new QSpacerItem(20, 267, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer, 1, 0, 1, 1);

    setWindowTitle(QString());

    tabPreferencesWidget->setPreferences(preferences);
    previewTextEdit->setPlainText(Constants::C_VCODESTYLEPREVIEWSNIPPET);

    decorateEditor(TextEditorSettings::fontSettings());

    connect(TextEditorSettings::instance(), &TextEditorSettings::fontSettingsChanged,
            this, &rCodeStylePreferencesWidget::decorateEditor);

    connect(m_preferences, &ICodeStylePreferences::currentTabSettingsChanged,
            this, &rCodeStylePreferencesWidget::updatePreview);

    setVisualizeWhitespace(true);

    updatePreview();
}

void rCodeStylePreferencesWidget::decorateEditor(const FontSettings &fontSettings) {
    previewTextEdit->textDocument()->setFontSettings(fontSettings);
    previewTextEdit->textDocument()->setSyntaxHighlighter(new VlangHighlighter());
    previewTextEdit->textDocument()->setIndenter(new VlangIndenter(previewTextEdit->textDocument()->document()));
}

void rCodeStylePreferencesWidget::setVisualizeWhitespace(bool on) {
    DisplaySettings displaySettings = previewTextEdit->displaySettings();
    displaySettings.m_visualizeWhitespace = on;
    previewTextEdit->setDisplaySettings(displaySettings);
}

void rCodeStylePreferencesWidget::updatePreview() {
    QTextDocument *doc = previewTextEdit->document();

    const TabSettings &ts = m_preferences
            ? m_preferences->currentTabSettings()
            : TextEditorSettings::codeStyle()->tabSettings();
    previewTextEdit->textDocument()->setTabSettings(ts);

    QTextBlock block = doc->firstBlock();
    QTextCursor tc = previewTextEdit->textCursor();
    tc.beginEditBlock();
    while (block.isValid()) {
        previewTextEdit->textDocument()->indenter()->indentBlock(block, QChar::Null, ts);
        block = block.next();
    }
    tc.endEditBlock();
}

SettingsWidget::SettingsWidget(QWidget *parent)
    : QWidget(parent)
{
    verticalLayout_2 = new QVBoxLayout(this);
    groupBox = new QGroupBox(this);
    verticalLayout = new QVBoxLayout(groupBox);
    formLayout = new QFormLayout();
    pathLabel = new QLabel(groupBox);

    formLayout->setWidget(0, QFormLayout::LabelRole, pathLabel);

    pathWidget = new Utils::PathChooser(groupBox);
    pathWidget->setExpectedKind(Utils::PathChooser::ExistingCommand);

    formLayout->setWidget(0, QFormLayout::FieldRole, pathWidget);

    verticalLayout->addLayout(formLayout);


    verticalLayout_2->addWidget(groupBox);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout_2->addItem(verticalSpacer);


    groupBox->setTitle(tr("V compiler"));
    pathLabel->setText(tr("Path"));
}

CodeStylePreferencesFactory::CodeStylePreferencesFactory() {

}

Utils::Id CodeStylePreferencesFactory::languageId() {
    return Constants::C_VLANGUAGE_ID;
}

QString CodeStylePreferencesFactory::displayName() {
    return Constants::C_VLANGUAGE_NAME;
}

ICodeStylePreferences *CodeStylePreferencesFactory::createCodeStyle() const {
    return new TextEditor::SimpleCodeStylePreferences();
}

QWidget *CodeStylePreferencesFactory::createEditor(ICodeStylePreferences *preferences, QWidget *parent) const {
    auto result = new rCodeStylePreferencesWidget(preferences, parent);
    result->layout()->setContentsMargins(0, 0, 0, 0);
    return result;
}

Indenter *CodeStylePreferencesFactory::createIndenter(QTextDocument *doc) const {
    return new VlangIndenter(doc);
}

QString CodeStylePreferencesFactory::snippetProviderGroupId() const {
    return Constants::C_VLANGSNIPPETSGROUP_ID;
}

QString CodeStylePreferencesFactory::previewText() const {
    return Constants::C_VCODESTYLEPREVIEWSNIPPET;
}

CodeStyleSettingsWidget::CodeStyleSettingsWidget()
{
    auto originalTabPreferences = VlangSettings::globalCodeStyle();
    m_vlangCodeStylePreferences = new SimpleCodeStylePreferences(this);
    m_vlangCodeStylePreferences->setDelegatingPool(originalTabPreferences->delegatingPool());
    m_vlangCodeStylePreferences->setTabSettings(originalTabPreferences->tabSettings());
    m_vlangCodeStylePreferences->setCurrentDelegate(originalTabPreferences->currentDelegate());
    m_vlangCodeStylePreferences->setId(originalTabPreferences->id());

    auto factory = TextEditorSettings::codeStyleFactory(Constants::C_VLANGUAGE_ID);

    auto editor = new CodeStyleEditor(factory, m_vlangCodeStylePreferences);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(editor);
}

void CodeStyleSettingsWidget::apply() {}

void CodeStyleSettingsWidget::finish() {}


} // namespace Internal
} // namespace Vcreator
