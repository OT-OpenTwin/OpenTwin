// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTCore/QueryDescription/DataPointDecoder.h"
#include "OTCore/ValueProcessing/ValueProcessing.h"

namespace ot {

	class OT_CORE_API_EXPORT DataLakeAccessCfg : public Serializable
	{
		OT_DECL_NOCOPY(DataLakeAccessCfg)
		OT_DECL_DEFMOVE(DataLakeAccessCfg)
	public:
		DataLakeAccessCfg() = default;
		virtual ~DataLakeAccessCfg() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void addCollection(const std::string& _collection, const std::string& _query) { m_collectionToQueryMap.insert_or_assign(_collection, _query); };
		void addCollection(const std::string& _collection, std::string&& _query) { m_collectionToQueryMap.insert_or_assign(_collection, std::move(_query)); };
		bool hasCollection(const std::string& _collection) const { return m_collectionToQueryMap.find(_collection) != m_collectionToQueryMap.end(); };
		const std::string& getQueryForCollection(const std::string& _collection) const { return m_collectionToQueryMap.at(_collection); };

		void addSeriesMetadata(const std::string& _series, JsonDocument&& _metadata) { m_seriesMetadataMap.insert_or_assign(_series, std::move(_metadata)); };
		bool hasSeriesMetadata(const std::string& _series) const { return m_seriesMetadataMap.find(_series) != m_seriesMetadataMap.end(); };
		JsonDocument& getSeriesMetadata(const std::string& _series) { return m_seriesMetadataMap.at(_series); };
		const JsonDocument& getSeriesMetadata(const std::string& _series) const { return m_seriesMetadataMap.at(_series); };

		void setProjection(const std::string& _projection) { m_projection = _projection; };
		void setProjection(std::string&& _projection) { m_projection = std::move(_projection); };
		const std::string& getProjection() const { return m_projection; };

		DataPointDecoder& addFieldDecoder(const std::string& _fieldKey, const DataPointDecoder& _decoder) { return m_fieldDecoders.insert_or_assign(_fieldKey, _decoder).first->second; };
		DataPointDecoder& addFieldDecoder(const std::string& _fieldKey, DataPointDecoder&& _decoder) { return m_fieldDecoders.insert_or_assign(_fieldKey, std::move(_decoder)).first->second; };
		bool hasFieldDecoder(const std::string& _fieldKey) const { return m_fieldDecoders.find(_fieldKey) != m_fieldDecoders.end(); };
		DataPointDecoder& getFieldDecoder(const std::string& _fieldKey) { return m_fieldDecoders.at(_fieldKey); };
		const DataPointDecoder& getFieldDecoder(const std::string& _fieldKey) const { return m_fieldDecoders.at(_fieldKey); };

		ValueProcessing& addValueTransformer(const std::string& _fieldKey, const ValueProcessing& _valueTransformer) { return m_valueTransformerByFieldKey.insert_or_assign(_fieldKey, _valueTransformer).first->second; };
		ValueProcessing& addValueTransformer(const std::string& _fieldKey, ValueProcessing&& _valueTransformer) { return m_valueTransformerByFieldKey.insert_or_assign(_fieldKey, std::move(_valueTransformer)).first->second; };
		bool hasValueTransformer(const std::string& _fieldKey) const { return m_valueTransformerByFieldKey.find(_fieldKey) != m_valueTransformerByFieldKey.end(); };
		ValueProcessing& getValueTransformer(const std::string& _fieldKey) { return m_valueTransformerByFieldKey.at(_fieldKey); };
		const ValueProcessing& getValueTransformer(const std::string& _fieldKey) const { return m_valueTransformerByFieldKey.at(_fieldKey); };

	private:
		std::map<std::string, std::string> m_collectionToQueryMap;
		std::map<std::string, JsonDocument> m_seriesMetadataMap;
		std::string m_projection;
		std::map<std::string, DataPointDecoder> m_fieldDecoders;
		std::map<std::string, ValueProcessing> m_valueTransformerByFieldKey;
	};

}