// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Logging/LogMessage.h"

// std header
#include <set>
#include <map>
#include <list>
#include <vector>
#include <sstream>
#include <unordered_set>
#include <unordered_map>

namespace ot {

	class OT_CORE_API_EXPORT LogMessageStream {
		OT_DECL_NOCOPY(LogMessageStream)
		OT_DECL_DEFMOVE(LogMessageStream)
		OT_DECL_NODEFAULT(LogMessageStream)
	public:
		template <typename T, typename = void>
		struct is_log_streamable
			: std::false_type {};

		template <typename T>
		struct is_log_streamable<T, std::void_t<decltype(std::declval<ot::LogMessageStream&>() << std::declval<T>())>>
			: std::true_type {};

		struct Pointer
		{
			OT_DECL_DEFCOPY(Pointer)
			OT_DECL_DEFMOVE(Pointer)
			OT_DECL_NODEFAULT(Pointer)

			explicit Pointer(const void* _ptr) : ptr(_ptr) {}

			const void* ptr;
		};

		explicit LogMessageStream(const std::string& _functionName, const LogFlags& _flags);
		~LogMessageStream() = default;

		LogMessageStream& operator << (bool _value);
		LogMessageStream& operator << (char _character);
		LogMessageStream& operator << (const char* const _plainText);
		LogMessageStream& operator << (const std::string& _plainText);
		LogMessageStream& operator << (int32_t _value);
		LogMessageStream& operator << (uint32_t _value);
		LogMessageStream& operator << (int64_t _value);
		LogMessageStream& operator << (uint64_t _value);
		LogMessageStream& operator << (float _value);
		LogMessageStream& operator << (double _value);

		LogMessageStream& operator << (const Serializable* const _serializableObject);
		LogMessageStream& operator << (const Pointer& _pointer);

		template <typename T, typename = std::enable_if_t<is_log_streamable<T>::value>>
		LogMessageStream& operator<<(const std::set<T>& _set);

		template <typename T, typename = std::enable_if_t<is_log_streamable<T>::value>>
		LogMessageStream& operator<<(const std::unordered_set<T>& _set);

		template <typename T, typename = std::enable_if_t<is_log_streamable<T>::value>>
		LogMessageStream& operator<<(const std::list<T>& _list);

		template <typename T, typename = std::enable_if_t<is_log_streamable<T>::value>>
		LogMessageStream& operator<<(const std::vector<T>& _vector);

		template <typename K, typename V, 
			typename = std::enable_if_t<is_log_streamable<K>::value>, 
			typename = std::enable_if_t<is_log_streamable<V>::value>>
		LogMessageStream& operator<<(const std::map<K, V>& _map);

		template <typename K, typename V,
			typename = std::enable_if_t<is_log_streamable<K>::value>,
			typename = std::enable_if_t<is_log_streamable<V>::value>>
			LogMessageStream & operator<<(const std::unordered_map<K, V>& _map);

		std::string getText() const;
		const std::string& getFunctionName() const { return m_functionName; };
		const LogFlags& getFlags() const { return m_logFlags; };

	private:
		template <typename T> void appendList(const T& _list);
		template <typename T> void appendMap(const T& _map);

		std::stringstream m_stream;
		std::string m_functionName;
		LogFlags m_logFlags;

	};

}

#include "OTCore/Logging/LogMessageStream.hpp"