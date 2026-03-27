// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTCore/QueryDescription/DataPointDecoder.h"
#include "OTCore/ValueProcessing/ValueProcessing.h"
#include <optional>

namespace ot {

	class OT_CORE_API_EXPORT DataLakeAccessCfg : public Serializable
	{
		OT_DECL_DEFMOVE(DataLakeAccessCfg)
	public:

		DataLakeAccessCfg(const DataLakeAccessCfg& _other);
		DataLakeAccessCfg& operator=(const DataLakeAccessCfg& _other);

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
		const std::map<std::string, std::string>& getQueriesByCollection() const { return m_collectionToQueryMap; }


		void addSeriesMetadata(const std::string& _series, JsonDocument&& _metadata) { m_seriesMetadataMap.insert_or_assign(_series, std::move(_metadata)); };
		bool hasSeriesMetadata(const std::string& _series) const { return m_seriesMetadataMap.find(_series) != m_seriesMetadataMap.end(); };
		JsonDocument& getSeriesMetadata(const std::string& _series) { return m_seriesMetadataMap.at(_series); };
		const JsonDocument& getSeriesMetadata(const std::string& _series) const { return m_seriesMetadataMap.at(_series); };

		void addFieldDecoderQuantity(const DataPointDecoder& _decoder) 
		{ 
			m_fieldDecodersQuantity.insert_or_assign(_decoder.getFieldName(), _decoder);
		};		

		std::optional<DataPointDecoder> getFieldDecoderQuantity(const std::string& _fieldKey) const 
		{ 
			auto decoder = m_fieldDecodersQuantity.find(_fieldKey);
			if (decoder != m_fieldDecodersQuantity.end())
			{
				return decoder->second;
			}
			else
			{
				return std::nullopt; 
			}

		};
		
		void addFieldDecoderParameter(const DataPointDecoder& _decoder) 
		{ 
			m_fieldDecodersParameter.insert_or_assign(_decoder.getFieldName(), _decoder);
		}
		std::optional<DataPointDecoder> getFieldDecoderParameter(const std::string& _fieldKey) 
		{
			auto decoder = m_fieldDecodersParameter.find(_fieldKey);
			if (decoder != m_fieldDecodersParameter.end())
			{
				return decoder->second;
			}
			else
			{
				return std::nullopt;
			}
		}

		void addFieldDecoderSeries(const DataPointDecoder& _decoder) 
		{ 
			m_fieldDecodersSeries.insert_or_assign(_decoder.getFieldName(), _decoder);
		}
		std::optional<DataPointDecoder> getFieldDecoderSeries(const std::string& _fieldKey) 
		{
			auto decoder = m_fieldDecodersSeries.find(_fieldKey);
			if (decoder != m_fieldDecodersSeries.end())
			{
				return decoder->second;
			}
			else
			{
				return std::nullopt;
			}
		}

		const std::map<std::string, DataPointDecoder>& getAllFieldDecoderParameter() const
		{
			return m_fieldDecodersParameter;
		}

		ValueProcessing& addParameterValueTransformer(const std::string& _fieldKey, const ValueProcessing& _valueTransformer) { return m_valueTransformerByFieldKeyParameter.insert_or_assign(_fieldKey, _valueTransformer).first->second; };
		ValueProcessing& addParameterValueTransformer(const std::string& _fieldKey, ValueProcessing&& _valueTransformer) { return m_valueTransformerByFieldKeyParameter.insert_or_assign(_fieldKey, std::move(_valueTransformer)).first->second; };
		bool hasParameterValueTransformer(const std::string& _fieldKey) const { return m_valueTransformerByFieldKeyParameter.find(_fieldKey) != m_valueTransformerByFieldKeyParameter.end(); };
		ValueProcessing& getParameterValueTransformer(const std::string& _fieldKey) { return m_valueTransformerByFieldKeyParameter.at(_fieldKey); };
		const ValueProcessing& getParameterValueTransformer(const std::string& _fieldKey) const { return m_valueTransformerByFieldKeyParameter.at(_fieldKey); };

		void addQuantityValueTransformer(const std::string& _fieldKey, const std::list<ValueProcessing>& _valueTransformers) { m_valueTransformerByFieldKeyQuantity.insert_or_assign(_fieldKey, _valueTransformers); }
		std::list<ValueProcessing> getQuantityValueTransformer(const std::string& _key) const { return m_valueTransformerByFieldKeyQuantity.at(_key); }
	
		const std::string& getCollectionName() const { return m_collectionNameBase; }
		void setCollectionName(const std::string& _collectionName) { m_collectionNameBase = _collectionName; }
	private:
		std::map<std::string, std::string> m_collectionToQueryMap;

		std::map<std::string, JsonDocument> m_seriesMetadataMap;
		
		std::string m_collectionNameBase;

		std::map<std::string, DataPointDecoder> m_fieldDecodersParameter;
		std::map<std::string, DataPointDecoder> m_fieldDecodersQuantity;
		std::map<std::string, DataPointDecoder> m_fieldDecodersSeries;
		
		std::map<std::string, ValueProcessing> m_valueTransformerByFieldKeyParameter;
		std::map<std::string, std::list<ValueProcessing>> m_valueTransformerByFieldKeyQuantity;
	};

}