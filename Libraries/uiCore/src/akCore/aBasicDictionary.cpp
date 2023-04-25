/*
 *	File:		aBasicDictionary.cpp
 *	Package:	akCore
 *
 *  Created on: August 07, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// uiCore header
#include <akCore/aBasicDictionary.h>

ak::aBasicDictionary::aBasicDictionary(LanguageKey _languageKey) : m_throwIfNoTranslationFound(false), m_currentLanguage(_languageKey) {}

ak::aBasicDictionary::aBasicDictionary(aBasicDictionary& _other) {
	m_currentLanguage = _other.m_currentLanguage;
	m_throwIfNoTranslationFound = _other.m_throwIfNoTranslationFound;
	for (auto keyItem : _other.m_translations) {
		std::map<LanguageKey, QString> * newMap = new std::map<LanguageKey, QString>();
		for (auto itm : *keyItem.second) {
			newMap->insert_or_assign(itm.first, itm.second);
		}
		m_translations.insert_or_assign(keyItem.first, newMap);
	}
}

ak::aBasicDictionary::~aBasicDictionary() {
	clearTranslations();
}

ak::aBasicDictionary& ak::aBasicDictionary::operator = (const aBasicDictionary& _other) {
	clearTranslations();

	m_currentLanguage = _other.m_currentLanguage;
	m_throwIfNoTranslationFound = _other.m_throwIfNoTranslationFound;
	for (auto keyItem : _other.m_translations) {
		std::map<LanguageKey, QString> * newMap = new std::map<LanguageKey, QString>();
		for (auto itm : *keyItem.second) {
			newMap->insert_or_assign(itm.first, itm.second);
		}
		m_translations.insert_or_assign(keyItem.first, newMap);
	}

	return *this;
}

// #########################################################################################

// Getter

const QString& ak::aBasicDictionary::translate(const QString& _key) {
	auto itm = m_translations.find(_key);
	if (itm == m_translations.end()) {
		assert(0);
		if (m_throwIfNoTranslationFound) { throw TranslationNotFoundException(); }
		return _key;
	}
	auto translation = itm->second->find(m_currentLanguage);
	if (translation == itm->second->end()) {
		assert(0);
		if (m_throwIfNoTranslationFound) { throw TranslationNotFoundException(); }
		return _key;
	}
	return translation->second;
}

const QString& ak::aBasicDictionary::operator[](const QString& _key) {
	auto itm = m_translations.find(_key);
	if (itm == m_translations.end()) {
		assert(0);
		if (m_throwIfNoTranslationFound) { throw TranslationNotFoundException(); }
		return _key;
	}
	auto translation = itm->second->find(m_currentLanguage);
	if (translation == itm->second->end()) {
		assert(0);
		if (m_throwIfNoTranslationFound) { throw TranslationNotFoundException(); }
		return _key;
	}
	return translation->second;
}

QString ak::aBasicDictionary::languageKeyToString(LanguageKey _languageKey) {
	switch (_languageKey)
	{
	case ak::aBasicDictionary::EN_GB: return "English (Britain)";
	case ak::aBasicDictionary::EN_US: return "English (USA)";
	case ak::aBasicDictionary::DE: return "Deutsch";
	case ak::aBasicDictionary::RUS: return QString::fromWCharArray(L"Русский");
	default:
		assert(0); // Not implemented language key
		return "EN-US";
	}
}

ak::aBasicDictionary::LanguageKey ak::aBasicDictionary::languageKeyFromString(const QString& _languageKey) {
	if (_languageKey == languageKeyToString(EN_US)) { return EN_US; }
	else if (_languageKey == languageKeyToString(EN_GB)) { return EN_GB; }
	else if (_languageKey == languageKeyToString(DE)) { return DE; }
	else if (_languageKey == languageKeyToString(RUS)) { return RUS; }
	else {
		assert(0);
		return EN_US;
	}
}

// #########################################################################################

// Setter

void ak::aBasicDictionary::setLanguage(const QString& _languageKey) {
	setLanguage(languageKeyFromString(_languageKey));
}

void ak::aBasicDictionary::clearTranslations(void) {
	for (auto itm : m_translations) {
		delete itm.second;
	}
	m_translations.clear();
}

void ak::aBasicDictionary::addTranslation(const QString& _key, LanguageKey _languageKey, const QString& _translation, bool _overwriteIfExists) {
	auto itm = m_translations.find(_key);
	if (itm == m_translations.end()) {
		std::map<LanguageKey, QString> * newMap = new std::map<LanguageKey, QString>();
		newMap->insert_or_assign(_languageKey, _translation);
		m_translations.insert_or_assign(_key, newMap);
	}
	else {
		auto translation = itm->second->find(_languageKey);
		if (_overwriteIfExists) { itm->second->insert_or_assign(_languageKey, _translation); }
		else if (translation == itm->second->end()) { itm->second->insert_or_assign(_languageKey, _translation); }
	}
}

void ak::aBasicDictionary::addTranslation(const QString& _key, LanguageKey _languageKey, const wchar_t * _translation, bool _overwriteIfExists) {
	addTranslation(_key, _languageKey, QString::fromWCharArray(_translation), _overwriteIfExists);
}

// #########################################################################################

// #########################################################################################

// #########################################################################################

ak::TranslationNotFoundException::TranslationNotFoundException() : std::exception("Translation not found") {}