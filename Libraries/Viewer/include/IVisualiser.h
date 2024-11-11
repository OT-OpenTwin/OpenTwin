#pragma once

class IVisualiser
{
public:
	virtual void visualise() = 0;
	bool isVisible() { return m_visible; }
	void setVisible(bool _visible) { m_visible = _visible;}
protected:
	bool m_visible = true;
};
