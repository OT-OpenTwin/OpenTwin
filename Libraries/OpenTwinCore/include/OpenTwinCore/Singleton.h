#pragma once

namespace ot {

	template <class T> class Singleton {
	public:
		//! @brief Return instance
		static T& instance() {
			static T instance;
			return instance;
		}
	protected:
		Singleton() = default;
		virtual ~Singleton() = default;

	private:
		Singleton(const Singleton&) = delete;
		Singleton& operator = (const Singleton&) = delete;
	};

}