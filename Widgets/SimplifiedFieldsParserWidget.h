#pragma once

#include "qsettings.h"
#include <QDialog>
#include <QStringList>

namespace Ui {
class SimplifiedFieldsParserWidget;
}

struct SimplifiedParseOptions
{
    bool joinNewlinesInsideDecl = true;      // Join multiline declarations into a single line
    bool trimSpacesAroundSemicolon = true;   // Normalize spaces around ';' (turn " ; " into ";")
    bool removeTrailingSemicolon = true;     // Remove trailing ';' from each output item
    bool ignoreQtMacros = true;              // Remove Qt access sections/macros and class wrappers
    bool removeComments = true;              // Remove //... and /*...*/ comments
    bool collapseMultSpaces = true;          // Replace repeated spaces/tabs with a single space
};

class SimplifiedFieldsParserWidget : public QDialog
{
    Q_OBJECT

public:
    explicit SimplifiedFieldsParserWidget(QWidget *parent = nullptr);
    ~SimplifiedFieldsParserWidget();

    void loadSettings(QSettings& s);
    void saveSettings(QSettings& s) const;

    const QStringList& getOutputList() const;

private:
    void parseInput();

    void bindUi();
    void syncOptionsFromUi();
    void applyOptionsToUi();

private:
    Ui::SimplifiedFieldsParserWidget *ui = nullptr;
    SimplifiedParseOptions m_options;
    QStringList m_outputList;
};
