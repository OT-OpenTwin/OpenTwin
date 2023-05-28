//! @file Owner.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/Singleton.h"
#include "OpenTwinCore/CoreTypes.h"
#include "OpenTwinCore/CoreAPIExport.h"
#include "OpenTwinCore/rJSON.h"

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
		T id(void) { return m_id; };

	private:
		T m_id;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @brief Owner type used for services
	typedef Owner<ot::serviceID_t> ServiceOwner_t;

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_CORE_API_EXPORT GlobalOwner : public ot::Owner<ot::serviceID_t>, ot::Singleton<ot::GlobalOwner> {
		OT_SINGLETON(ot::GlobalOwner)
	public:
		//! @brief Retreives the global owner id from the provided json object
		//! @param _object The json object containing the global owner id
		//! @param _id Where to write data to
		static bool getIdFromJson(OT_rJSON_val& _object, ot::serviceID_t& _id);

		//! @brief Retreives the global owner from the provided json object
		//! @param _object The json object containing the global owner id
		static ot::ServiceOwner_t ownerFromJson(OT_rJSON_val& _object);

		//! @brief Add the global owner id to the provided json object
		//! @param _doc The json document
		//! @param _object The object to add the data to
		void addToJsonObject(OT_rJSON_doc& _doc, OT_rJSON_val& _object);

	private:
		GlobalOwner() : Owner<ot::serviceID_t>(ot::invalidServiceID) {};
		virtual ~GlobalOwner() {};

		GlobalOwner(const GlobalOwner&) = delete;
		GlobalOwner& operator = (const GlobalOwner&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @brief Manage objects that have an owner
	template<class K, class V> class OwnerManagerTemplate {
	public:

		//! @brief Store the provided object for the given owner
		//! The manager takes ownership of the object
		void store(const ot::Owner<K>& _owner, V* _obj) { objectList(_owner)->push_back(_obj); };

		//! @brief Will clean up the memory while removing all entries
		void free(void) {
			for (auto it : m_data) {
				for (auto obj : *it.second) { delete obj.second; };
				delete it.second;
			}
			m_data.clear();
		}

		//! @brief Clean up the memory for the given owner and remove its entries
		//! @param _owner The object owner
		void free(const ot::Owner<K>& _owner) {
			auto it = m_data.find(_owner);
			if (it != m_data.end()) {
				for (auto obj : *it->second) delete obj;
				delete it->second;
				
				remove(_owner);
			}
		}

		//! @brief Remove all entries for the given owner
		//! Callee takes ownership of affected objects
		//! @param _owner The object owner
		void remove(const ot::Owner<K>& _owner) { m_data.erase(_owner); };

		//! @brief Remove all entries for all owners
		//! Callee takes ownership of all objects stored
		void removeAll(void) {
			for (auto it : m_data) { delete it.second; }
			m_data.clear();
		}

		//! @brief Returns true if there exists an entry for the provided owner
		bool contains(const ot::Owner<K>& _owner) { return m_data.count(_owner) > 0; };

		//! @brief Return the owner data
		std::list<V*>* const operator[](const ot::Owner<K>& _owner) { return objectList(_owner); };

	private:

		//! @brief Return object list for the given owner
		std::list<V*>* objectList(const ot::Owner<K>& _owner) {
			auto it = m_data.find(_owner);
			if (it == m_data.end()) {
				std::list<V*>* newList = new std::list<V*>;
				m_data.insert_or_assign(_owner, newList);
				return newList;
			}
			else {
				return it->second;
			}
		};

		//! @brief Owner -> ObjectList map
		std::map<ot::Owner<K>, std::list<V*> *> m_data;

	};

	template <class V> class ServiceOwnerManagerTemplate : public OwnerManagerTemplate<ot::serviceID_t, V> {};
}
