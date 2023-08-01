//! @file Margins.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date July 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	template <class T>
	class __declspec(dllexport)	MarginsTemplate : public ot::Serializable {
	public:
		MarginsTemplate() {};
		MarginsTemplate(T _top, T _right, T _bottom, T _left) : m_top(_top), m_right(_right), m_bottom(_bottom), m_left(_left) {};
		MarginsTemplate(const MarginsTemplate<T>& _other) : m_top(_other.m_top), m_right(_other.m_right), m_bottom(_other.m_bottom), m_left(_other.m_left) {};
		virtual ~MarginsTemplate() {};

		MarginsTemplate<T>& operator = (const MarginsTemplate<T>& _other);
		bool operator == (const MarginsTemplate<T>& _other);
		bool operator != (const MarginsTemplate<T>& _other);

		void set(T _top, T _right, T _bottom, T _left);
		
		void setTop(T _top) { m_top = _top; };
		T top(void) const { return m_top; };

		void setRight(T _right) { m_right = _right; };
		T right(void) const { return m_right; };

		void setBottom(T _bottom) { m_bottom = _bottom; };
		T bottom(void) const { return m_bottom; };

		void setLeft(T _left) { m_left = _left; };
		T left(void) const { return m_left; };

	protected:
		T m_top;
		T m_right;
		T m_bottom;
		T m_left;
	};

	// #########################################################################################################################################################################################################

	class OT_GUI_API_EXPORT Margins : public MarginsTemplate<int> {
	public:
		Margins() : MarginsTemplate<int>(0, 0, 0, 0) {};
		Margins(int _top, int _right, int _bottom, int _left) : MarginsTemplate<int>(_top, _right, _bottom, _left) {};
		Margins(const Margins& _other) : MarginsTemplate<int>(_other.m_top, _other.m_right, _other.m_bottom, _other.m_left) {};
		virtual ~Margins() {};
		
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const override;
		virtual void setFromJsonObject(OT_rJSON_val& _jsonObject) override;
	};

	// #########################################################################################################################################################################################################

	class OT_GUI_API_EXPORT MarginsF : public MarginsTemplate<float> {
	public:
		MarginsF() : MarginsTemplate<float>(0.f, 0.f, 0.f, 0.f) {};
		MarginsF(float _top, float _right, float _bottom, float _left) : MarginsTemplate<float>(_top, _right, _bottom, _left) {};
		MarginsF(const MarginsF& _other) : MarginsTemplate<float>(_other.m_top, _other.m_right, _other.m_bottom, _other.m_left) {};
		virtual ~MarginsF() {};

		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const override;
		virtual void setFromJsonObject(OT_rJSON_val& _jsonObject) override;
	};

	// #########################################################################################################################################################################################################

	class OT_GUI_API_EXPORT MarginsD : public MarginsTemplate<double> {
	public:
		MarginsD() : MarginsTemplate<double>(0., 0., 0., 0.) {};
		MarginsD(double _top, double _right, double _bottom, double _left) : MarginsTemplate<double>(_top, _right, _bottom, _left) {};
		MarginsD(const MarginsD& _other) : MarginsTemplate<double>(_other.m_top, _other.m_right, _other.m_bottom, _other.m_left) {};
		virtual ~MarginsD() {};

		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const override;
		virtual void setFromJsonObject(OT_rJSON_val& _jsonObject) override;
	};

}

template <class T>
ot::MarginsTemplate<T>& ot::MarginsTemplate<T>::operator = (const MarginsTemplate<T>& _other) {
	m_top = _other.m_top;
	m_right = _other.m_right;
	m_bottom = _other.m_bottom;
	m_left = _other.m_left;
	return *this;
}

template <class T>
bool ot::MarginsTemplate<T>::operator == (const MarginsTemplate<T>& _other) {
	return m_top == _other.m_top && m_right == _other.m_right && m_bottom == _other.m_bottom && m_left == _other.m_left;
}

template <class T>
bool ot::MarginsTemplate<T>::operator != (const MarginsTemplate<T>& _other) {
	return m_top != _other.m_top || m_right != _other.m_right || m_bottom != _other.m_bottom || m_left != _other.m_left;
}

template <class T>
void ot::MarginsTemplate<T>::set(T _top, T _right, T _bottom, T _left) {
	m_top = _top;
	m_right = _right;
	m_bottom = _bottom;
	m_left = _left;
}