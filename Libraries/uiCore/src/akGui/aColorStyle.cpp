/*
 *	File:		aColorStyle.cpp
 *	Package:	akGui
 *
 *  Created on: April 09, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akGui/aColorStyle.h>
#include <akCore/aException.h>

// Qt header
#include <qdir.h>					// QDir
#include <qfile.h>					// QFile

QString ak::aColorStyle::getFilePath(
	const QString &			_fileName
) const {
	try {
		// Search in all directories for the file
		for (int i = 0; i < m_directories.size(); i++) {
			try {
				// Check if the file exist at the current directory
				QString f(m_directories.at(i));
				f.append("ColorStyles/");
				f.append(getColorStyleName());
				f.append("/");
				f.append(_fileName);

				QFile file(f);
				if (file.exists()) {
					QStringList lst = f.split("\\");
					QString out = "";
					for (int c = 0; c < lst.length(); c++) {
						out.append(lst.at(c));
						if (c < lst.length() - 1) { out.append("/"); }
					}
					return out;
				}
			}
			catch (...) {
				throw aException(QString(QString("Failed to check file\nFile: ") + QString(_fileName) + QString("\nPath: ") +
					QString(m_directories.at(i))).toStdString().c_str(), "Check file");
			}
		}
		throw aException(QString(QString("The requested file: \"") + QString(_fileName) + QString("\" does not exist in the icon search directories")).toStdString(),
			"Find file", aException::exceptionType::FileNotFound);
	}
	catch (const aException & e) { throw aException(e, "ak::aColorStyle::getFilePath()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aColorStyle::getFilePath()"); }
	catch (...) { throw aException("Unknown error", "ak::aColorStyle::getFilePath()"); }
}
