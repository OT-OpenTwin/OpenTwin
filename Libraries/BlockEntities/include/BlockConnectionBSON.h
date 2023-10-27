#pragma once
#include <string>
#include "OTGui/GraphicsConnectionCfg.h"
#include "SerializeWithBSON.h"
#include "OpenTwinCore/CoreTypes.h"

namespace ot
{
	class BlockConnectionBSON : public SerializeWithBSON
	{
	public:
		BlockConnectionBSON(const ot::GraphicsConnectionCfg& connection);
		BlockConnectionBSON() {};
		const ot::GraphicsConnectionCfg& getConnection() const;
		void setConnection(const ot::GraphicsConnectionCfg& connection);
		virtual bsoncxx::builder::basic::document SerializeBSON() const override;
		virtual void DeserializeBSON(bsoncxx::v_noabi::types::b_document& storage) override;
	private:
		ot::GraphicsConnectionCfg _connection;
	};
}