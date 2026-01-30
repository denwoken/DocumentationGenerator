#include "SimplifiedFieldsParserWidget.h"
#include "ui_SimplifiedFieldsParserWidget.h"

#include <QCheckBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QSignalBlocker>
#include <QDebug>



// NOTE: Patterns are kept as-is (as requested). Only moved to static const to avoid
// re-creating them on every parse call.
static const QRegularExpression kRePublicSlots      (R"(public\s*(slots)?\s*:)");
static const QRegularExpression kReProtectedSlots   (R"(protected\s*(slots)?\s*:)");
static const QRegularExpression kRePrivateSlots     (R"(private\s*(slots)?\s*:)");
static const QRegularExpression kReSignals          (R"(signals\s*:)");
static const QRegularExpression kReQObject          (R"(\s*Q_OBJECT\s*)");
static const QRegularExpression kReQSlot            (R"(\s*Q_SLOT\s*)");
static const QRegularExpression kReQSignal          (R"(\s*Q_SIGNAL\s*)");
static const QRegularExpression kReClassOrStructHdr (R"(\s*(class|struct)\s*\w*\s*[^(\{|;)]+(\{|;)\s*)");
static const QRegularExpression kReClassEnd         (R"(\s*}\s*;\s*)");

// Comments (kept your pattern; split into separate option)
static const QRegularExpression kReComments         (R"(\/\/[^\n]*\n|\/\*[\s\S]*?\*\/)");

// Formatting helpers
static const QRegularExpression kReSpacesAroundSemi (R"(\s*;\s*)");
static const QRegularExpression kReNewlinesOnly     (R"(\n+)");          // Fix: remove only real newlines
static const QRegularExpression kReMultSpaces       (R"([ \t]{2,})");



SimplifiedFieldsParserWidget::SimplifiedFieldsParserWidget(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SimplifiedFieldsParserWidget)
{
    ui->setupUi(this);

    bindUi();
    syncOptionsFromUi();

    // Initial parse
    parseInput();
}

SimplifiedFieldsParserWidget::~SimplifiedFieldsParserWidget()
{
    delete ui;
}

void SimplifiedFieldsParserWidget::bindUi()
{
    // OK / Cancel
    connect(ui->pushButton_ok, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->pushButton_cancel, &QPushButton::clicked, this, &QDialog::reject);

    // Any option change triggers re-parse. This keeps UI responsive and always up-to-date.
    auto reparse = [this]() {
        syncOptionsFromUi();
        parseInput();
    };

    connect(ui->checkJoinMultiline,           &QCheckBox::toggled, this, reparse);
    connect(ui->checkTrimAroundSemicolon,     &QCheckBox::toggled, this, reparse);
    connect(ui->checkRemoveTrailingSemicolon, &QCheckBox::toggled, this, reparse);
    connect(ui->checkIgnoreQtMacros,          &QCheckBox::toggled, this, reparse);
    connect(ui->checkCollapseMultSpaces,      &QCheckBox::toggled, this, reparse);

    // NEW: checkbox for comment removal (add it in .ui with this objectName)
    //      name suggestion: checkRemoveComments
    connect(ui->checkRemoveComments,          &QCheckBox::toggled, this, reparse);

    // Input changes
    connect(ui->plainTextEditInput, &QPlainTextEdit::textChanged,
            this, &SimplifiedFieldsParserWidget::parseInput);

    // Manual parse button
    connect(ui->pushButtonParse, &QPushButton::clicked,
            this, &SimplifiedFieldsParserWidget::parseInput);
}

void SimplifiedFieldsParserWidget::syncOptionsFromUi()
{
    // Read UI state once and store it in the options struct.
    // This avoids reading UI widgets repeatedly during parsing.
    m_options.joinNewlinesInsideDecl  = ui->checkJoinMultiline->isChecked();
    m_options.trimSpacesAroundSemicolon = ui->checkTrimAroundSemicolon->isChecked();
    m_options.removeTrailingSemicolon = ui->checkRemoveTrailingSemicolon->isChecked();
    m_options.ignoreQtMacros          = ui->checkIgnoreQtMacros->isChecked();
    m_options.collapseMultSpaces      = ui->checkCollapseMultSpaces->isChecked();
    m_options.removeComments          = ui->checkRemoveComments->isChecked();
}

void SimplifiedFieldsParserWidget::parseInput()
{
    QString inputText = ui->plainTextEditInput->toPlainText();
    inputText.replace("\r\n", "\n");
    inputText.replace('\r', '\n');
    inputText = inputText.trimmed();

    if (inputText.isEmpty()) {
        ui->listWidgetOutput->clear();
        m_outputList.clear();
        return;
    }

    // 1) Optional: remove comments first.
    //    It is safer to remove comments BEFORE removing Qt sections/macros,
    //    because commented-out code may contain tokens that would confuse the cleanup stage.
    if (m_options.removeComments) {
        inputText.replace(kReComments, "");
    }

    // 2) Optional: remove Qt sections/macros and class/struct wrappers.
    //    This is tailored for your simplified parsing flow.
    if (m_options.ignoreQtMacros) {
        inputText.replace(kRePublicSlots, "");
        inputText.replace(kReProtectedSlots, "");
        inputText.replace(kRePrivateSlots, "");
        inputText.replace(kReSignals, "");
        inputText.replace(kReQObject, "");
        inputText.replace(kReQSlot, "");
        inputText.replace(kReQSignal, "");
        inputText.replace(kReClassOrStructHdr, "");
        inputText.replace(kReClassEnd, "");
    }

    // 3) Basic guard: we expect declarations to be separated by ';'.
    //    If the text does not end with ';', parsing by split(';') will drop the last item.
    //    We don't "fix" it automatically here to avoid surprising behavior.
    if (!inputText.trimmed().endsWith(';')) {
        qWarning() << "Simplified parser expects input to end with ';' to keep the last declaration.";
        // Still continue: user might be typing. The output will just be partial.
    }

    // 4) Formatting options
    if (m_options.trimSpacesAroundSemicolon) {
        // Normalize delimiter spacing: "foo()  ;  bar();" -> "foo();bar();"
        inputText.replace(kReSpacesAroundSemi, ";");
    }

    if (m_options.joinNewlinesInsideDecl) {
        // Join multiline declarations. IMPORTANT:
        // Your previous regex R"(\n*)" matched empty strings, which effectively broke the whole text.
        // Here we remove only actual newline sequences.
        inputText.replace(kReNewlinesOnly, " ");
    }

    if (m_options.collapseMultSpaces) {
        // Collapse repeated spaces/tabs to a single space.
        inputText.replace(kReMultSpaces, " ");
    }

    inputText = inputText.trimmed();

    // 5) Split declarations by ';'
    m_outputList = inputText.split(';', Qt::SkipEmptyParts);

    // Clean final items
    for (QString &s : m_outputList) {
        s = s.trimmed();
        if (!m_options.removeTrailingSemicolon && !s.isEmpty())
            s += ';';
    }

    // 6) Push to UI
    ui->listWidgetOutput->clear();
    ui->listWidgetOutput->addItems(m_outputList);
}








static const char* kSettingsGroup = "SimplifiedFieldsParserWidget";
// Keys for parser options
static const char* kKeyJoinMultiline       = "joinNewlinesInsideDecl";
static const char* kKeyTrimAroundSemicolon = "trimSpacesAroundSemicolon";
static const char* kKeyRemoveSemicolon     = "removeTrailingSemicolon";
static const char* kKeyIgnoreQtMacros      = "ignoreQtMacros";
static const char* kKeyRemoveComments      = "removeComments";
static const char* kKeyCollapseMultSpaces  = "collapseMultSpaces";

void SimplifiedFieldsParserWidget::loadSettings(QSettings& s)
{
    // Load persisted options and UI state.
    s.beginGroup(kSettingsGroup);

    // Read options with defaults taken from current struct values.
    m_options.joinNewlinesInsideDecl   = s.value(kKeyJoinMultiline,       m_options.joinNewlinesInsideDecl).toBool();
    m_options.trimSpacesAroundSemicolon= s.value(kKeyTrimAroundSemicolon, m_options.trimSpacesAroundSemicolon).toBool();
    m_options.removeTrailingSemicolon  = s.value(kKeyRemoveSemicolon,     m_options.removeTrailingSemicolon).toBool();
    m_options.ignoreQtMacros           = s.value(kKeyIgnoreQtMacros,      m_options.ignoreQtMacros).toBool();
    m_options.removeComments           = s.value(kKeyRemoveComments,      m_options.removeComments).toBool();
    m_options.collapseMultSpaces       = s.value(kKeyCollapseMultSpaces,  m_options.collapseMultSpaces).toBool();
    s.endGroup();

    // Apply loaded options into UI checkboxes (blocking signals).
    applyOptionsToUi();
}

void SimplifiedFieldsParserWidget::saveSettings(QSettings& s) const
{
    // Persist options and UI state.
    s.beginGroup(kSettingsGroup);
    s.setValue(kKeyJoinMultiline,       m_options.joinNewlinesInsideDecl);
    s.setValue(kKeyTrimAroundSemicolon, m_options.trimSpacesAroundSemicolon);
    s.setValue(kKeyRemoveSemicolon,     m_options.removeTrailingSemicolon);
    s.setValue(kKeyIgnoreQtMacros,      m_options.ignoreQtMacros);
    s.setValue(kKeyRemoveComments,      m_options.removeComments);
    s.setValue(kKeyCollapseMultSpaces,  m_options.collapseMultSpaces);
    s.endGroup();
}

const QStringList &SimplifiedFieldsParserWidget::getOutputList() const{
    return m_outputList;
}

void SimplifiedFieldsParserWidget::applyOptionsToUi()
{
    // Push current m_options into checkboxes without triggering re-parse signals.
    ui->checkJoinMultiline->setChecked(m_options.joinNewlinesInsideDecl);
    ui->checkTrimAroundSemicolon->setChecked(m_options.trimSpacesAroundSemicolon);
    ui->checkRemoveTrailingSemicolon->setChecked(m_options.removeTrailingSemicolon);
    ui->checkIgnoreQtMacros->setChecked(m_options.ignoreQtMacros);
    ui->checkCollapseMultSpaces->setChecked(m_options.collapseMultSpaces);
    ui->checkRemoveComments->setChecked(m_options.removeComments);
}



















