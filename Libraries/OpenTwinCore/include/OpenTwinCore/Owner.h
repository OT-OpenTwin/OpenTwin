//! @file Owner.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/CoreTypes.h"
#include "OpenTwinCore/CoreAPIExport.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/Logger.h"

// std header
#include <string>
#include <list>
#include <map>

namespace ot {

	template <class T>
	class __declspec(dllexport) Owner {
	public:
		Owner(const T& _id) : m_id(_id) {};
		Owner(const Owner<T>& _other) : m_id(_other.m_id) {};
		Owner& operator = (const Owner<T>& _other) { m_id = _other.m_id; return *this; };
		virtual ~Owner() {};

		bool operator == (T _id) const { return m_id == _id; };
		bool operator == (const Owner<T>& _other) const { return m_id == _other.m_id; };
		bool operator != (T _id) const { return m_id != _id; };
		bool operator != (const Owner<T>& _other) const { return m_id != _other.m_id; };
		bool operator < (T _id) const { return m_id <= _id; };
		bool operator < (const Owner<T>& _other) const { return m_id < _other.m_id; };
		bool operator <= (T _id) const { return m_id <= _id; };
		bool operator <= (const Owner<T>& _other) const { return m_id <= _other.m_id; };
		bool operator > (T _id) const { return m_id > _id; };
		bool operator > (const Owner<T>& _other) const { return m_id > _other.m_id; };
		bool operator >= (T _id) const { return m_id >= _id; };
		bool operator >= (const Owner<T>& _other) const { return m_id >= _other.m_id; };

		void setId(const T& _id) { m_id = _id; };
		T getId(void) const { return m_id; };

	private:
		T m_id;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

		// Konkretes Problem: BlockEntities muss eine Nachricht an das UI schicken und Owner wird mittlerweile als Member vorrausgesetzt. 
		// Darum muss es möglich sein einen Owner Member zu haben, welcher aber auch serializable ist. Aktuell ist nur GlobalOwner serializeable.
		// Besser: Owner serializable und GlobalOwner nur als Singleton
		// Außerdem: 
		// - Redundante Kommentare. An der Stelle addToJsonObject wäre gerade ein besserer Kommentar nötig
		// - Overload von addToJsonObject mit nur dem Document?
		// - Warum muss Owner ein Template sein?
		// - Warum nicht von Serializable ableiten?
		// 
}
