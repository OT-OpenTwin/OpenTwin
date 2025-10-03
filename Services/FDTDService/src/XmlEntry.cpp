#include "XmlEntry.h"

XmlEntry::XmlEntry()
{
}

XmlEntry::~XmlEntry()
{
}

XmlEntry::XmlEntry(std::string tag, std::map<std::string, std::string> attributes, std::string text)
	: m_tag(tag), m_attributes(attributes), m_text(text)
{
}

XmlEntry* XmlEntry::addNode(const XmlEntry& node) {
	m_nodes.push_back(node);
	return &m_nodes.back();
}