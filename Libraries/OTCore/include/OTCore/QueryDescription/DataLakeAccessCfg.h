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

		void addCollection(const std::string& _collection, const std::string& _query);
		void addCollection(const std::string& _collection, std::string&& _query);
		bool hasCollection(const std::string& _collection) const;
		const std::string& getQueryForCollection(const std::string& _collection) const;
		const std::map<std::string, std::string>& getQueriesByCollection() const;


		void addSeriesMetadata(const std::string& _series, JsonDocument&& _metadata);
		bool hasSeriesMetadata(const std::string& _series) const ;
		JsonDocument& getSeriesMetadata(const std::string& _series);
		const JsonDocument& getSeriesMetadata(const std::string& _series) const;

		void addFieldDecoderQuantity(const DataPointDecoder& _decoder);		

		std::optional<DataPointDecoder> getFieldDecoderQuantity(const std::string& _fieldKey) const;
		std::optional<DataPointDecoder> getFieldDecoderQuantityByLabel(const std::string& _label) const;
		
		size_t getNumberOfFieldDecoderParameter() const;

		void addFieldDecoderParameter(const DataPointDecoder& _decoder);

		std::optional<DataPointDecoder> getFieldDecoderParameter(const std::string& _fieldKey) const;
		std::optional<DataPointDecoder> getFieldDecoderParameterByLabel(const std::string& _label) const;

		void addFieldDecoderSeries(const DataPointDecoder& _decoder);

		std::optional<DataPointDecoder> getFieldDecoderSeries(const std::string& _fieldKey) const;

		const std::map<std::string, DataPointDecoder>& getAllFieldDecoderParameter() const;
		const std::map<std::string, DataPointDecoder*>& getAllFieldDecoderParameterByLabel() const;
		const std::map<std::string, DataPointDecoder*>& getAllFieldDecoderQuantityByLabel() const;
		const std::map<std::string, DataPointDecoder*>& getAllFieldDecoderSeriesByLabel() const;

		ValueProcessing& addParameterValueTransformer(const std::string& _fieldKey, const ValueProcessing& _valueTransformer);
		ValueProcessing& addParameterValueTransformer(const std::string& _fieldKey, ValueProcessing&& _valueTransformer);
		bool hasParameterValueTransformer(const std::string& _fieldKey) const;
		ValueProcessing& getParameterValueTransformer(const std::string& _fieldKey);
		const ValueProcessing& getParameterValueTransformer(const std::string& _fieldKey) const;

		void addQuantityValueTransformer(const std::string& _fieldKey, const std::list<ValueProcessing>& _valueTransformers);
		std::list<ValueProcessing> getQuantityValueTransformer(const std::string& _key) const;
	
		const std::string& getCollectionName() const;
		void setCollectionName(const std::string& _collectionName);

	private:
		std::map<std::string, std::string> m_collectionToQueryMap;

		std::map<std::string, JsonDocument> m_seriesMetadataMap;
		
		std::string m_collectionNameBase;

		std::map<std::string, DataPointDecoder> m_fieldDecodersParameter;
		std::map<std::string, DataPointDecoder> m_fieldDecodersQuantity;
		std::map<std::string, DataPointDecoder> m_fieldDecodersSeries;
		
		std::map<std::string, DataPointDecoder*> m_fieldDecodersParameterByLabel;
		std::map<std::string, DataPointDecoder*> m_fieldDecodersQuantityByLabel;
		std::map<std::string, DataPointDecoder*> m_fieldDecodersSeriesByLabel;

		std::map<std::string, ValueProcessing> m_valueTransformerByFieldKeyParameter;
		std::map<std::string, std::list<ValueProcessing>> m_valueTransformerByFieldKeyQuantity;
	};

}