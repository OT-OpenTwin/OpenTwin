#pragma once

#include <string>
#include <stdexcept>
#include <vector>
#include <map>


class XmlEntry {
public:
	XmlEntry();
	XmlEntry(std::string tag, std::map<std::string, std::string> attributes = {}, std::string text = "");
	virtual ~XmlEntry();

	const std::string& getTag() const { return m_tag; }
	const std::string& getText() const { return m_text; }
	const std::map<std::string, std::string>& getAttributes() const { return m_attributes; }
	const std::vector<XmlEntry>& getNodes() const { return m_nodes; }

	void setTag(const std::string& tag) { m_tag = tag; };
	void setText(const std::string& text) { m_text = text; }
	void setAttributes(const std::string& key, const std::string& value) { m_attributes[key] = value; }

	XmlEntry* addNode(const XmlEntry& node);

private:
	std::string m_tag;
	std::map<std::string, std::string> m_attributes;
	std::string m_text;
	std::vector<XmlEntry> m_nodes;
};