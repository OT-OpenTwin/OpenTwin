//! @file Plot1DCurveCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Plot1DCurveCfg.h"
#include "OTGui/Painter2DFactory.h"

std::string ot::Plot1DCurveCfg::toString(Symbol _symbol) {
	switch (_symbol) {
	case ot::Plot1DCurveCfg::NoSymbol: return "None";
	case ot::Plot1DCurveCfg::Circle: return "None";
	case ot::Plot1DCurveCfg::Square: return "None";
	case ot::Plot1DCurveCfg::Diamond: return "None";
	case ot::Plot1DCurveCfg::TriangleUp: return "None";
	case ot::Plot1DCurveCfg::TriangleDown: return "None";
	case ot::Plot1DCurveCfg::TriangleLeft: return "None";
	case ot::Plot1DCurveCfg::TriangleRight: return "None";
	case ot::Plot1DCurveCfg::Cross: return "None";
	case ot::Plot1DCurveCfg::XCross: return "None";
	case ot::Plot1DCurveCfg::HLine: return "None";
	case ot::Plot1DCurveCfg::VLine: return "None";
	case ot::Plot1DCurveCfg::Star6: return "None";
	case ot::Plot1DCurveCfg::Star8: return "None";
	case ot::Plot1DCurveCfg::Hexagon: return "None";
	default:
		OT_LOG_EAS("Unknown plot curve symbol (" + std::to_string(static_cast<int>(_symbol)) + ")");
		return "None";
	}
}

ot::Plot1DCurveCfg::Symbol ot::Plot1DCurveCfg::stringToSymbol(const std::string& _symbol) {
	if (_symbol == toString(NoSymbol)) return NoSymbol;
	else if (_symbol == toString(Circle)) return Circle;
	else if (_symbol == toString(Square)) return Square;
	else if (_symbol == toString(Diamond)) return Diamond;
	else if (_symbol == toString(TriangleUp)) return TriangleUp;
	else if (_symbol == toString(TriangleDown)) return TriangleDown;
	else if (_symbol == toString(TriangleLeft)) return TriangleLeft;
	else if (_symbol == toString(TriangleRight)) return TriangleRight;
	else if (_symbol == toString(Cross)) return Cross;
	else if (_symbol == toString(XCross)) return XCross;
	else if (_symbol == toString(HLine)) return HLine;
	else if (_symbol == toString(VLine)) return VLine;
	else if (_symbol == toString(Star6)) return Star6;
	else if (_symbol == toString(Star8)) return Star8;
	else if (_symbol == toString(Hexagon)) return Hexagon;
	else {
		OT_LOG_EAS("Unknown plot curve symbol \"" + _symbol + "\"");
		return NoSymbol;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor / Destructor

ot::Plot1DCurveCfg::Plot1DCurveCfg() : 
	Plot1DCurveCfg(0, 0, std::string()) 
{}

ot::Plot1DCurveCfg::Plot1DCurveCfg(UID _id, UID _version, const std::string& _name) :
	BasicEntityInformation(_name, _id, _version), m_navigationId(0), m_visible(true), m_dimmed(false),
	m_linePen(1., Color(DefaultColor::Red)), m_pointsSize(5), m_pointsSymbol(NoSymbol),
	m_pointsFillPainter(new FillPainter2D(DefaultColor::Lime)), m_pointsOulinePen(1., DefaultColor::Lime)
{}

ot::Plot1DCurveCfg::Plot1DCurveCfg(const Plot1DCurveCfg& _other) : m_pointsFillPainter(nullptr) {
	this->operator=(_other);
}

ot::Plot1DCurveCfg::Plot1DCurveCfg(Plot1DCurveCfg&& _other) noexcept :
	m_pointsFillPainter(nullptr)
{
	this->operator=(std::move(_other));
}

ot::Plot1DCurveCfg::~Plot1DCurveCfg() {}

// ###########################################################################################################################################################################################################################################################################################################################

// Opearator

ot::Plot1DCurveCfg& ot::Plot1DCurveCfg::operator=(Plot1DCurveCfg&& _other) noexcept {
	if (this != &_other) {
		BasicEntityInformation::operator=(std::move(_other));
		
		m_navigationId = _other.m_navigationId;

		m_title = std::move(_other.m_title);

		m_visible = _other.m_visible;
		m_dimmed = _other.m_dimmed;

		m_linePen = std::move(_other.m_linePen);

		m_pointsSymbol = _other.m_pointsSymbol;
		m_pointsSize = _other.m_pointsSize;
		m_pointsOulinePen = std::move(_other.m_pointsOulinePen);

		if (m_pointsFillPainter) {
			delete m_pointsFillPainter;
		}
		m_pointsFillPainter = _other.m_pointsFillPainter;
		_other.m_pointsFillPainter = nullptr;

		m_queryInformation = std::move(_other.m_queryInformation);
	}

	return *this;
}

ot::Plot1DCurveCfg& ot::Plot1DCurveCfg::operator=(const Plot1DCurveCfg& _other) {
	BasicEntityInformation::operator=(_other);

	if (this != &_other) {
		m_navigationId = _other.m_navigationId;
		
		m_title = _other.m_title;
		
		m_visible = _other.m_visible;
		m_dimmed = _other.m_dimmed;

		m_linePen = _other.m_linePen;
		
		m_queryInformation = _other.m_queryInformation;

		m_pointsSymbol = _other.m_pointsSymbol;
		m_pointsSize = _other.m_pointsSize;
		m_pointsOulinePen = _other.m_pointsOulinePen;
		this->setPointsFillPainter(_other.getPointsFillPainter()->createCopy());
	}

	return *this;
}

ot::Plot1DCurveCfg& ot::Plot1DCurveCfg::operator=(const BasicEntityInformation& _other) {
	BasicEntityInformation::operator=(_other);
	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void ot::Plot1DCurveCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	
	BasicEntityInformation::addToJsonObject(_object, _allocator);

	OTAssertNullptr(m_pointsFillPainter);

	_object.AddMember("NavID", m_navigationId, _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);

	_object.AddMember("Visible", m_visible, _allocator);
	_object.AddMember("Dimmed", m_dimmed, _allocator);
	
	_object.AddMember("LinePen", JsonObject(m_linePen, _allocator), _allocator);
	_object.AddMember("PointSymbol", JsonString(Plot1DCurveCfg::toString(m_pointsSymbol), _allocator), _allocator);
	_object.AddMember("PointsSize", m_pointsSize, _allocator);
	_object.AddMember("PointOutlinePen", JsonObject(m_pointsOulinePen, _allocator), _allocator);
	_object.AddMember("PointFillPainter", JsonObject(m_pointsFillPainter, _allocator), _allocator);

	_object.AddMember("QueryInformation", JsonObject(m_queryInformation, _allocator), _allocator);
}

void ot::Plot1DCurveCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	BasicEntityInformation::setFromJsonObject(_object);

	m_navigationId = json::getUInt64(_object, "NavID");
	m_title = json::getString(_object, "Title");

	m_visible = json::getBool(_object, "Visible");
	m_dimmed = json::getBool(_object, "Dimmed");

	m_linePen.setFromJsonObject(json::getObject(_object, "LinePen"));
	m_pointsSymbol = Plot1DCurveCfg::stringToSymbol(json::getString(_object, "PointSymbol"));
	m_pointsSize = json::getInt(_object, "PointsSize");
	m_pointsOulinePen.setFromJsonObject(json::getObject(_object, "PointOutlinePen"));

	m_queryInformation.setFromJsonObject(json::getObject(_object, "QueryInformation"));

	Painter2D* p = Painter2DFactory::create(json::getObject(_object, "PointFillPainter"));
	if (p) {
		this->setPointsFillPainter(p);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::Plot1DCurveCfg::setPointsFillColor(const Color& _color) {
	this->setPointsFillPainter(new FillPainter2D(_color));
}

void ot::Plot1DCurveCfg::setPointsFillPainter(Painter2D* _painter) {
	OTAssertNullptr(_painter);
	if (_painter == m_pointsFillPainter) {
		return;
	}

	if (m_pointsFillPainter) {
		delete m_pointsFillPainter;
	}

	m_pointsFillPainter = _painter;
}

void ot::Plot1DCurveCfg::setQueryInformation(QueryInformation _queryInformation)
{
	m_queryInformation = _queryInformation;
}

const ot::QueryInformation& ot::Plot1DCurveCfg::getQueryInformation() const
{
	return m_queryInformation;
}
