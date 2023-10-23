#pragma once
#include <string>
#include "OTGui/GraphicsPackage.h"
#include "SerializeWithBSON.h"
#include "OpenTwinCore/CoreTypes.h"

namespace ot
{
	class __declspec(dllexport) BlockConnection : public SerializeWithBSON
	{
	public:
		BlockConnection(const ot::GraphicsConnectionPackage::ConnectionInfo& connection);
		BlockConnection() {};
		const ot::GraphicsConnectionPackage::ConnectionInfo& getConnection() const;
		void setConnection(const ot::GraphicsConnectionPackage::ConnectionInfo& connection);
		virtual bsoncxx::builder::basic::document SerializeBSON() const override;
		virtual void DeserializeBSON(bsoncxx::v_noabi::types::b_document& storage) override;
	private:
		ot::GraphicsConnectionPackage::ConnectionInfo _connection;
	};
}