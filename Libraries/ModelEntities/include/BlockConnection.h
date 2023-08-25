#pragma once
#include <string>
#include "SerializeWithBSON.h"
#include "OpenTwinCore/CoreTypes.h"

namespace ot
{
	class __declspec(dllexport) BlockConnection : public SerializeWithBSON
	{
		public:
			BlockConnection(const std::string& connectorOrigin, const std::string& connectorDestination, ot::UID idDestination);
			BlockConnection();
			std::string getConnectorOrigin() const;
			std::string getConnectorDestination() const;
			ot::UID getIDDestination() const;
			virtual bsoncxx::builder::basic::document SerializeBSON() const override;
			virtual void DeserializeBSON(bsoncxx::v_noabi::types::b_document& storage) override;

		private:
			std::string _connectorOrigin;
			std::string _connectorDestination;
			ot::UID _idDestination;

	};
}