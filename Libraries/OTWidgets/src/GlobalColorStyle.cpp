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
#include <QtCore/qdiriterator.h>
#include <QtWidgets/qstyle.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qstylefactory.h>

ot::GlobalColorStyle& ot::GlobalColorStyle::instance(void) {
	static GlobalColorStyle g_instance;
	return g_instance;
}

void ot::GlobalColorStyle::addStyle(const ColorStyle& _style, bool _replace) {
	if (!_replace && this->hasStyle(_style.colorStyleName())) {
		OT_LOG_E("Color style already exists (Name: \"" + _style.colorStyleName() + "\")");
	}

	if (_style.colorStyleName().empty()) {
		m_emptyStyle = _style;
	}

	m_styles.insert_or_assign(_style.colorStyleName(), _style);
	ColorStyle& cs = m_styles.at(_style.colorStyleName());
	this->evaluateColorStyleFiles(cs);
	this->evaluateStyleSheetMacros(cs);

	if (m_currentStyle == _style.colorStyleName()) {
		Q_EMIT currentStyleChanged(cs);
	}
}

void ot::GlobalColorStyle::addStyle(const QByteArray& _rawStyle, bool _replace, bool _apply) {
	if (_rawStyle.isEmpty()) return;
	ColorStyle newStyle;
	
	if (!newStyle.setupFromFile(_rawStyle)) return;

	this->addStyle(newStyle, _replace);

	if (_apply) {
		this->setCurrentStyle(newStyle.colorStyleName());
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
	if (m_app) {
		m_app->setStyleSheet(it->second.styleSheet());
	}
	OT_LOG_D("Current color style changed \"" + m_currentStyle + "\"");
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

void ot::GlobalColorStyle::scanForStyleFiles(void) {
	for (const QString& pth : m_styleRootSearchPaths) {
		QDirIterator it(pth, QStringList() << "*.otcsf", QDir::Files, QDirIterator::Subdirectories);

		while (it.hasNext()) {
			QString f = it.next();
			QFile file(f);
			if (!file.open(QIODevice::ReadOnly)) continue;
			QByteArray data = file.readAll();
			file.close();

			this->addStyle(data, true);
		}
	}
}

void ot::GlobalColorStyle::evaluateColorStyleFiles(ColorStyle& _style) const {
	QString rootPath = this->styleRootPath(_style.colorStyleName());
	std::map<std::string, QString> data = _style.getFiles();
	for (auto& it : data) {
		it.second = rootPath + it.second;
	}
	_style.setFiles(data);
}

void ot::GlobalColorStyle::evaluateStyleSheetMacros(ColorStyle& _style) const {
	QString rootPath = this->styleRootPath(_style.colorStyleName());
	QString evaluatedSheet = _style.styleSheet();
	evaluatedSheet.replace(OT_COLORSTYLE_FILE_MACRO_Root, rootPath);

	_style.setStyleSheet(evaluatedSheet);
}

void ot::GlobalColorStyle::setApplication(QApplication* _application) {
	if (m_app == _application) return;
	OTAssertNullptr(_application);
	m_app = _application;
}

ot::GlobalColorStyle::GlobalColorStyle() : m_app(nullptr) {
	m_emptyStyle.setColorStyleName("");
	m_styles.insert_or_assign(m_emptyStyle.colorStyleName(), m_emptyStyle);
}
