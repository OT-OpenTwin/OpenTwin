#pragma once
#pragma warning(disable:4251)

#include <string>
#include "SerializeWithBSON.h"

namespace ot
{
	enum ConnectorType {In = 0, Out = 1, InOptional = 2, UNKNOWN = -1};

	class __declspec(dllexport) Connector : public SerializeWithBSON
	{
	public:
		Connector();
		Connector(ConnectorType type, const std::string& connectorName, const std::string& connectorTitle);
		bool operator==(const Connector& other) { return _connectorName == other._connectorName; }
		
		const ConnectorType getConnectorType() const { return _connectorType; }
		const std::string& getConnectorName() const { return _connectorName; }
		const std::string& getConnectorTitle() const { return _connectorTitle; }
		
		virtual bsoncxx::builder::basic::document SerializeBSON() const  override;
		virtual void DeserializeBSON(bsoncxx::v_noabi::types::b_document& storage) override;

	private:
		ConnectorType _connectorType;
		std::string _connectorName;
		std::string _connectorTitle;

	};
}