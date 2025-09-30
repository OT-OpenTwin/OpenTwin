//! @file Plot1DCurveCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Plot1DCurveCfg.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/StyleRefPainter2D.h"

std::string ot::Plot1DCurveCfg::toString(Symbol _symbol) {
	switch (_symbol) {
	case ot::Plot1DCurveCfg::NoSymbol: return "None";
	case ot::Plot1DCurveCfg::Circle: return "Circle";
	case ot::Plot1DCurveCfg::Square: return "Square";
	case ot::Plot1DCurveCfg::Diamond: return "Diamond";
	case ot::Plot1DCurveCfg::TriangleUp: return "Triangle Up";
	case ot::Plot1DCurveCfg::TriangleDown: return "Triangle Down";
	case ot::Plot1DCurveCfg::TriangleLeft: return "Triangle Left";
	case ot::Plot1DCurveCfg::TriangleRight: return "Triangle Right";
	case ot::Plot1DCurveCfg::Cross: return "Cross (+)";
	case ot::Plot1DCurveCfg::XCross: return "X Cross (X)";
	case ot::Plot1DCurveCfg::HLine: return "Horizontal Line";
	case ot::Plot1DCurveCfg::VLine: return "Vertical Line";
	case ot::Plot1DCurveCfg::Star6: return "Star 6";
	case ot::Plot1DCurveCfg::Star8: return "Star 8";
	case ot::Plot1DCurveCfg::Hexagon: return "Hexagon";
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
	m_linePen(1., new StyleRefPainter2D(ColorStyleValueEntry::PlotCurve)), m_pointSize(5), m_pointInterval(1), m_pointSymbol(NoSymbol),
	m_pointFillPainter(new StyleRefPainter2D(ColorStyleValueEntry::PlotCurveSymbol)), 
	m_pointOulinePen(1., new StyleRefPainter2D(ColorStyleValueEntry::PlotCurveSymbol))
{}

ot::Plot1DCurveCfg::Plot1DCurveCfg(const Plot1DCurveCfg& _other) : m_pointFillPainter(nullptr) {
	this->operator=(_other);
}

ot::Plot1DCurveCfg::Plot1DCurveCfg(Plot1DCurveCfg&& _other) noexcept :
	m_pointFillPainter(nullptr)
{
	this->operator=(std::move(_other));
}

ot::Plot1DCurveCfg::~Plot1DCurveCfg() {
	if (m_pointFillPainter) {
		delete m_pointFillPainter;
		m_pointFillPainter = nullptr;
	}
}

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

		m_pointSize = _other.m_pointSize;
		m_pointInterval = _other.m_pointInterval;
		m_pointSymbol = _other.m_pointSymbol;
		m_pointOulinePen = std::move(_other.m_pointOulinePen);

		if (m_pointFillPainter) {
			delete m_pointFillPainter;
		}
		m_pointFillPainter = _other.m_pointFillPainter;
		_other.m_pointFillPainter = nullptr;

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
		
		m_pointSize = _other.m_pointSize;
		m_pointInterval = _other.m_pointInterval;
		m_pointSymbol = _other.m_pointSymbol;
		m_pointOulinePen = _other.m_pointOulinePen;

		m_queryInformation = _other.m_queryInformation;

		this->setPointFillPainter(_other.getPointFillPainter()->createCopy());
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

	OTAssertNullptr(m_pointFillPainter);

	_object.AddMember("NavID", m_navigationId, _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);

	_object.AddMember("Visible", m_visible, _allocator);
	_object.AddMember("Dimmed", m_dimmed, _allocator);
	
	_object.AddMember("LinePen", JsonObject(m_linePen, _allocator), _allocator);

	_object.AddMember("PointSymbol", JsonString(Plot1DCurveCfg::toString(m_pointSymbol), _allocator), _allocator);
	_object.AddMember("PointSize", m_pointSize, _allocator);
	_object.AddMember("PointInterval", m_pointInterval, _allocator);
	_object.AddMember("PointOutlinePen", JsonObject(m_pointOulinePen, _allocator), _allocator);
	_object.AddMember("PointFillPainter", JsonObject(m_pointFillPainter, _allocator), _allocator);

	_object.AddMember("QueryInformation", JsonObject(m_queryInformation, _allocator), _allocator);
}

void ot::Plot1DCurveCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	BasicEntityInformation::setFromJsonObject(_object);

	m_navigationId = json::getUInt64(_object, "NavID");
	m_title = json::getString(_object, "Title");

	m_visible = json::getBool(_object, "Visible");
	m_dimmed = json::getBool(_object, "Dimmed");

	m_linePen.setFromJsonObject(json::getObject(_object, "LinePen"));

	m_pointSymbol = Plot1DCurveCfg::stringToSymbol(json::getString(_object, "PointSymbol"));
	m_pointSize = json::getInt(_object, "PointSize");
	m_pointInterval = json::getInt(_object, "PointInterval");
	m_pointOulinePen.setFromJsonObject(json::getObject(_object, "PointOutlinePen"));

	m_queryInformation.setFromJsonObject(json::getObject(_object, "QueryInformation"));

	Painter2D* p = Painter2DFactory::create(json::getObject(_object, "PointFillPainter"));
	if (p) {
		this->setPointFillPainter(p);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::Plot1DCurveCfg::setPointFillColor(DefaultColor _color) {
	this->setPointFillPainter(new FillPainter2D(_color));
}

void ot::Plot1DCurveCfg::setPointFillColor(const Color& _color) {
	this->setPointFillPainter(new FillPainter2D(_color));
}

void ot::Plot1DCurveCfg::setPointFillColor(ColorStyleValueEntry _styleReference) {
	this->setPointFillPainter(new StyleRefPainter2D(_styleReference));
}

void ot::Plot1DCurveCfg::setPointFillPainter(Painter2D* _painter) {
	OTAssertNullptr(_painter);
	if (_painter == m_pointFillPainter) {
		return;
	}

	if (m_pointFillPainter) {
		delete m_pointFillPainter;
	}

	m_pointFillPainter = _painter;
}
