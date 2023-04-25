/*
 *	File:		aAbstractDictionary.h
 *	Package:	akCore
 *
 *  Created on: August 07, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// uiCore header
#include <akCore/aAbstractDictionary.h>
#include <akCore/globalDataTypes.h>

// Qt header
#include <qstring.h>

// C++ header
#include <map>
#include <exception>

namespace ak {

	class UICORE_API_EXPORT aBasicDictionary : public aAbstractDictionary {
	public:

		enum LanguageKey {
			EN_GB,
			EN_US,
			DE,
			RUS
		};

		//! @brief Default constructor
		aBasicDictionary(LanguageKey _languageKey = EN_US);

		//! @brief Copy constructor
		//! @param _other The other basic dictionary
		aBasicDictionary(aBasicDictionary& _other);

		//! @brief Destructor
		virtual ~aBasicDictionary();

		//! @brief Assingment operator
		//! @param _other The other basic dictionary
		aBasicDictionary& operator = (const aBasicDictionary& _other);

		// #########################################################################################

		// Getter

		virtual const QString& translate(const QString& _key) override;

		const LanguageKey& currentLanguage(void) const { return m_currentLanguage; }

		bool isThrowIfNoTranslationFound(void) const { return m_throwIfNoTranslationFound; }

		const QString& operator[](const QString& _key);

		static QString languageKeyToString(LanguageKey _languageKey);

		static LanguageKey languageKeyFromString(const QString& _languageKey);

		// #########################################################################################

		// Setter
		
		void setLanguage(LanguageKey _languageKey) { m_currentLanguage = _languageKey; }

		void setLanguage(const QString& _languageKey);

		void clearTranslations(void);

		void setThrowIfNoTranslationFound(bool _isThrow) { m_throwIfNoTranslationFound = _isThrow; }

		void addTranslation(const QString& _key, LanguageKey _languageKey, const QString& _translation, bool _overwriteIfExists = false);

		void addTranslation(const QString& _key, LanguageKey _languageKey, const wchar_t * _translation, bool _overwriteIfExists = false);

	protected:

		bool												m_throwIfNoTranslationFound;
		LanguageKey											m_currentLanguage;
		std::map<QString, std::map<LanguageKey, QString> *>	m_translations;		//! Map containing all translations: Key -> LanguageKey -> Translation

	private:
	};

	class TranslationNotFoundException : public std::exception {
	public:
		TranslationNotFoundException();
	};

}