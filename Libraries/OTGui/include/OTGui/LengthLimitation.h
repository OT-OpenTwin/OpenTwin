#pragma once

// OpenTwin header
#include "OpenTwinCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	template <class T>
	class __declspec(dllexport) LengthLimitationTemplate : public ot::Serializable {
	public:
		LengthLimitationTemplate() : m_min((T)0), m_minSet(false), m_max((T)0), m_maxSet(false) {};
		LengthLimitationTemplate(T _min, T _max) : m_min(_min), m_minSet(_min > 0), m_max(_max), m_maxSet(_max > 0) {};
		LengthLimitationTemplate(const LengthLimitationTemplate<T>& _other) : m_min(_other.m_min), m_minSet(_other.m_minSet), m_max(_other.m_max), m_maxSet(_other.m_maxSet) {};
		virtual ~LengthLimitationTemplate() {};

		LengthLimitationTemplate<T>& operator = (const LengthLimitationTemplate<T>& _other);
		bool operator == (const LengthLimitationTemplate<T>& _other);
		bool operator != (const LengthLimitationTemplate<T>& _other);

		inline void set(T _min, T _max);

		void setMin(T _min) { m_min = _min; m_minSet = (_min > 0); };
		T min(void) const { return m_min; };
		bool isMinSet(void) const { return m_minSet; };

		void setMax(T _max) { m_max = _max; m_maxSet = (_max > 0); };
		T max(void) const { return m_max; };
		bool isMaxSet(void) const { return m_maxSet; };

	protected:
		T		m_min;
		bool	m_minSet;
		T		m_max;
		bool	m_maxSet;
	};

	// #########################################################################################################################################################################################################

	class OT_GUI_API_EXPORT LengthLimitation : public LengthLimitationTemplate<int> {
	public:
		LengthLimitation() : LengthLimitationTemplate<int>() {};
		LengthLimitation(int _min, int _max) : LengthLimitationTemplate<int>(_min, _max) {};

		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const override;
		virtual void setFromJsonObject(OT_rJSON_val& _jsonObject) override;
	};

	// #########################################################################################################################################################################################################

	class OT_GUI_API_EXPORT LengthLimitationF : public LengthLimitationTemplate<float> {
	public:
		LengthLimitationF() : LengthLimitationTemplate<float>() {};
		LengthLimitationF(float _min, float _max) : LengthLimitationTemplate<float>(_min, _max) {};

		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const override;
		virtual void setFromJsonObject(OT_rJSON_val& _jsonObject) override;
	};

	// #########################################################################################################################################################################################################

	class OT_GUI_API_EXPORT LengthLimitationD : public LengthLimitationTemplate<double> {
	public:
		LengthLimitationD() : LengthLimitationTemplate<double>() {};
		LengthLimitationD(double _min, double _max) : LengthLimitationTemplate<double>(_min, _max) {};

		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const override;
		virtual void setFromJsonObject(OT_rJSON_val& _jsonObject) override;
	};

}

template <class T>
ot::LengthLimitationTemplate<T>& ot::LengthLimitationTemplate<T>::operator = (const LengthLimitationTemplate<T>& _other) {
	m_min = _other.m_min;
	m_minSet = _other.m_minSet;
	m_max = _other.m_max;
	m_maxSet = _other.m_maxSet;
	return *this;
}

template <class T>
bool ot::LengthLimitationTemplate<T>::operator == (const LengthLimitationTemplate<T>& _other) {
	return m_min == _other.m_min && m_minSet == _other.m_minSet && m_max == _other.m_max && m_maxSet == _other.m_maxSet;
}

template <class T>
bool ot::LengthLimitationTemplate<T>::operator != (const LengthLimitationTemplate<T>& _other) {
	return m_min != _other.m_min || m_minSet != _other.m_minSet || m_max != _other.m_max || m_maxSet != _other.m_maxSet;
}

template <class T>
void ot::LengthLimitationTemplate<T>::set(T _min, T _max) {
	m_min = _min;
	m_max = _max;
	m_minSet = true;
	m_maxSet = true;
}