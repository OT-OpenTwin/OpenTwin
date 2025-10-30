// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/SyntaxHighlighterRule.h"

ot::SyntaxHighlighterRule::SyntaxHighlighterRule() {
	m_painter = new FillPainter2D;
}

ot::SyntaxHighlighterRule::SyntaxHighlighterRule(const std::string& _regularExpression, Painter2D* _painter)
	: m_regex(_regularExpression), m_painter(_painter)
{
	OTAssertNullptr(m_painter);
}

ot::SyntaxHighlighterRule::SyntaxHighlighterRule(const SyntaxHighlighterRule& _other) 
	: m_painter(nullptr)
{
	*this = _other;
}

ot::SyntaxHighlighterRule::SyntaxHighlighterRule(SyntaxHighlighterRule&& _other) noexcept :
	m_painter(nullptr)
{
	*this = std::move(_other);
}

ot::SyntaxHighlighterRule::~SyntaxHighlighterRule() {
	if (m_painter) {
		delete m_painter;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Operators

ot::SyntaxHighlighterRule& ot::SyntaxHighlighterRule::operator = (const SyntaxHighlighterRule& _other) {
	if (this != &_other) {
		m_regex = _other.m_regex;
		m_font = _other.m_font;

		OTAssertNullptr(_other.m_painter);
		this->setPainter(_other.m_painter->createCopy());
	}

	return *this;
}

ot::SyntaxHighlighterRule& ot::SyntaxHighlighterRule::operator=(SyntaxHighlighterRule&& _other) noexcept {
	if (this != &_other) {
		if (m_painter) {
			delete m_painter;
		}
		m_painter = _other.m_painter;
		_other.m_painter = nullptr;

		m_regex = std::move(_other.m_regex);
		m_font = std::move(_other.m_font);
	}

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void ot::SyntaxHighlighterRule::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Regex", JsonString(m_regex, _allocator), _allocator);

	JsonObject painterObj;
	if (m_painter) {
		m_painter->addToJsonObject(painterObj, _allocator);
	}
	else {
		FillPainter2D().addToJsonObject(painterObj, _allocator);
	}
	
	_object.AddMember("Painter", painterObj, _allocator);

	JsonObject fontObj;
	m_font.addToJsonObject(fontObj, _allocator);
	_object.AddMember("Font", fontObj, _allocator);
}

void ot::SyntaxHighlighterRule::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_regex = json::getString(_object, "Regex");

	ConstJsonObject painterObj = json::getObject(_object, "Painter");
	this->setPainter(Painter2DFactory::create(painterObj));

	ConstJsonObject fontObj = json::getObject(_object, "Font");
	m_font.setFromJsonObject(fontObj);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::SyntaxHighlighterRule::setPainter(Painter2D* _painter) {
	OTAssertNullptr(_painter);
	if (m_painter == _painter) {
		return;
	}
	
	if (m_painter) {
		delete m_painter;
	}

	m_painter = _painter;
}
