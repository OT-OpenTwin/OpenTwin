//! @file GlobalColorStyle.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/ColorStyleTypes.h"

// Qt header
#include <QtCore/qdir.h>

ot::GlobalColorStyle& ot::GlobalColorStyle::instance(void) {
	static GlobalColorStyle g_instance;
	return g_instance;
}

void ot::GlobalColorStyle::addStyle(const ColorStyle& _style, bool _replace) {
	if (_style.name().empty()) {
		OT_LOG_E("Color style name may not be empty");
		return;
	}
	if (!_replace && this->hasStyle(_style.name())) {
		OT_LOG_E("Color style already exists (Name: \"" + _style.name() + "\")");
	}

	m_styles.insert_or_assign(_style.name(), _style);
	this->evaluateStyleSheetMacros(m_styles[_style.name()]);

	if (m_currentStyle == _style.name()) {
		Q_EMIT currentStyleChanged(m_styles[_style.name()]);
	}
}

bool ot::GlobalColorStyle::hasStyle(const std::string& _name) const {
	return m_styles.find(_name) != m_styles.end();
}

const ot::ColorStyle& ot::GlobalColorStyle::getStyle(const std::string& _name, const ColorStyle& _default) const {
	const auto& it = m_styles.find(_name);
	if (it == m_styles.end()) return _default;
	else return it->second;
}

bool ot::GlobalColorStyle::setCurrentStyle(const std::string& _styleName) {
	const auto& it = m_styles.find(_styleName);
	if (it == m_styles.end()) {
		OT_LOG_E("Color style does not exist (Name: \"" + _styleName + "\")");
		return false;
	}

	m_currentStyle = _styleName;
	Q_EMIT currentStyleChanged(it->second);
	return true;
}

const ot::ColorStyle& ot::GlobalColorStyle::getCurrentStyle(void) const {
	if (m_currentStyle.empty()) {
		return m_emptyStyle;
	}
	const auto& it = m_styles.find(m_currentStyle);
	if (it == m_styles.end()) {
		OT_LOG_E("Invalid data");
		return m_emptyStyle;
	}
	return it->second;
}

bool ot::GlobalColorStyle::addStyleRootSearchPath(const QString& _path) {
	if (_path.isEmpty()) {
		OT_LOG_WA("Empty search path provided. Ignoring");
		return false;
	}

	QString path = _path;
	path.replace("\\", "/");
	if (!path.endsWith("/")) path.append("/");

	for (const QString& pth : m_styleRootSearchPaths) {
		if (pth == path) return true;
	}

	QDir dir(path);
	if (dir.exists(path)) {
		m_styleRootSearchPaths.append(path);
		OT_LOG_D("Added style image search path: \"" + path.toStdString() + "\"");
		return true;
	}
	else {
		OT_LOG_D("Style iamge search path does not exist. Ignoring. Path: \"" + path.toStdString() + "\"");
		return false;
	}
}

QString ot::GlobalColorStyle::styleRootPath(const std::string& _styleName) const {
	for (const QString& searchPath : m_styleRootSearchPaths) {
		QDir dir(searchPath + QString::fromStdString(_styleName));
		if (dir.exists()) {
			return searchPath + QString::fromStdString(_styleName);
		}
	}
	OT_LOG_W("Root path for style \"" + _styleName + "\" not found");
	return QString();
}

ot::GlobalColorStyle::GlobalColorStyle() {
	m_emptyStyle.setName("");
	m_styles.insert_or_assign(m_emptyStyle.name(), m_emptyStyle);
}

void ot::GlobalColorStyle::evaluateStyleSheetMacros(ColorStyle& _style) {
	QString rootPath = this->styleRootPath(_style.name());
	QString evaluatedSheet = _style.styleSheet();
	evaluatedSheet.replace(OT_COLORSTYLE_FILE_MACRO_Root, rootPath);

	_style.setStyleSheet(evaluatedSheet);
}
