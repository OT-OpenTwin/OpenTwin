#pragma once

// OToolkit header
#include "AbstractTool.h"

// Qt header
#include <QtCore/qstringlist.h>

class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;

class TextFinder : public OToolkitAPI::AbstractTool {
public:
	TextFinder();
	virtual ~TextFinder();

	//! @brief Returns the unique name of this tool
	//! @note If another tool with the same name was registered before, the instance of this tool will be destroyed
	virtual QString toolName(void) const override { return "TextFinder"; };

	QWidget* widget(void) { return m_centralLayoutW; };

private:
	QStringList m_whitelist;
	QStringList m_blacklist;
	QStringList m_allowedFiles;

	QWidget* m_centralLayoutW;
	QVBoxLayout* m_centralLayout;
	QGridLayout* m_filterLayout;
	QHBoxLayout* m_resultLayout;
	QHBoxLayout* m_buttonLayout;

	QPushButton* m_buttonFind;

};