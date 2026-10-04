// @otlicense

#include "stdafx.h"

// Model service header
#include "Model.h"
#include "Application.h"
#include "MDF4/MDF4Parser.h"

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTModelEntities/MDF/EntityMDFFile.h"
#include "OTModelEntities/MDF/EntityMDFChannel.h"
#include "OTModelEntities/MDF/EntityMDFChannelData.h"

// MDF lib
#include <mdf/mdfreader.h>
#include <mdf/mdflogstream.h>

ot::NewModelStateInfo ot::MDF4Parser::parse(ot::TemporaryFile&& _file, const std::list<std::string>& _existingMDFFileEntities)
{
	MDF4Parser parser(std::move(_file), _existingMDFFileEntities);
	try
	{
		if (parser.parse())
		{
			return parser.getNewEntities();
		}
		else
		{
			return NewModelStateInfo();
		}
	}
	catch (const std::exception& _e)
	{
		OT_LOG_ES("Failed to parse MDF4 file: " << _e.what());
		return NewModelStateInfo();
	}
	catch (...)
	{
		OT_LOG_E("Failed to parse MDF4 file: Unknown error");
		return NewModelStateInfo();
	}
}

void ot::MDF4Parser::log(const StyledTextBuilder& _message)
{
	Application* app = Application::instance();
	OTAssertNullptr(app);
	auto uiComponent = app->getUiComponent();
	if (!uiComponent)
	{
		OT_LOG_E("Cannot display message since ui component is not available");
		return;
	}

	uiComponent->displayStyledMessage(_message);
}

void ot::MDF4Parser::log(const std::string& _message)
{
	MDF4Parser::log(StyledTextBuilder() << "[mdf] " << _message);
}

void ot::MDF4Parser::logWarning(const std::string& _message)
{
	MDF4Parser::log(StyledTextBuilder() << "[mdf] [" << StyledText::Warning << "Warning" << StyledText::ClearStyle << "] " << _message);
}

void ot::MDF4Parser::logError(const std::string& _message)
{
	MDF4Parser::log(StyledTextBuilder() << "[mdf] [" << StyledText::Error << "Error" << StyledText::ClearStyle << "] " << _message);
}

ot::MDF4Parser::MDF4Parser(TemporaryFile&& _file, const std::list<std::string>& _existingMDFFileEntities)
	: m_file(std::move(_file))
{

}

bool ot::MDF4Parser::parse()
{
	Application* app = Application::instance();
	OTAssertNullptr(app);
	Model* model = app->getModel();
	if (!model)
	{
		OT_LOG_E("Cannot parse MDF4 file since model is not available");
		return false;
	}

	// Helper
	const std::string fileName = m_file.getUniqueName();
	const std::filesystem::path tmpFilePath = m_file.getFilePath();
	const std::string tmpFilePathString = tmpFilePath.string();

	this->log("Processing MDF file: \"" + fileName + "\"\n");

	OT_LOG_DS("Starting to parse MDF4 file: \"" << m_file.getUniqueName() << "\"");
	OT_LOG_DS("+ Temporary MDF4 file stored at: \"" + tmpFilePathString << "\"");

	// Initialize reader
	mdf::MdfReader reader(tmpFilePathString);
	if (!reader.IsOk())
	{
		OT_USER_LOG_E("Failed to initialize MDF4 file reader: " + tmpFilePathString);
		return false;
	}

	const mdf::MdfFile* readerFile = reader.GetFile();
	if (!readerFile)
	{
		OT_USER_LOG_E("Failed to get MDF4 file object: " + tmpFilePathString);
		return false;
	}

	// Read topology and meta-data
	if (!reader.ReadEverythingButData())
	{
		OT_USER_LOG_E("Failed to read MDF4 file: " + tmpFilePathString);
		return false;
	}

	// Fetch data groups
	mdf::DataGroupList dataGroups;
	readerFile->DataGroups(dataGroups);

	this->log("Found " + std::to_string(dataGroups.size()) + " data group" + (dataGroups.size() == 1 ? " " : "s ") + "in MDF4 file \"" + fileName + "\".\n");

	// Go through all data groups
	for (mdf::IDataGroup* dataGroup : dataGroups)
	{
		if (!dataGroup)
		{
			this->logWarning("Encountered null data group in MDF4 file \"" + fileName + "\". Skipping.\n");
			continue;
		}

		std::list<std::pair<std::unique_ptr<EntityMDFChannel>, mdf::ChannelObserverPtr>> datasets;

		// Go through all channel groups in the data group
		for (mdf::IChannelGroup* channelGroup : dataGroup->ChannelGroups())
		{
			if (!channelGroup)
			{
				this->logWarning("Encountered null channel group in MDF4 file \"" + fileName + "\". Skipping.\n");
				continue;
			}
			
			// Go through all channels in the channel group
			for (mdf::IChannel* channel : channelGroup->Channels())
			{
				if (!channel)
				{
					this->logWarning("Encountered null channel in MDF4 file \"" + fileName + "\". Skipping.\n");
					continue;
				}

				//MDF4Dataset dataset(channel->Name());

				// Get the meta-data for the channel
				/*mdf::IMetaData* metaData = channel->MetaData();

				for (const mdf::ETag& tag : metaData->Properties())
				{
					std::string tagDescription = tag.Description();
					std::string tagLanguage = tag.Language();
					std::string tagName = tag.Name();
					std::string tagType = tag.Type();
					std::string tagUnit = tag.Unit();
					std::string tagValue = tag.Value<std::string>();

					mdf::ETagDataType tagDataType = tag.DataType();


				}*/
				
				// Read samples
				auto obs = mdf::CreateChannelObserver(*dataGroup, *channelGroup, *channel);
				//datasets.emplace_back(std::move(dataset), std::move(obs));
			}
		}

		// Ensure data group is read
		if (!dataGroup->IsRead())
		{
			if (!reader.ReadData(*dataGroup))
			{
				this->logWarning("Failed to read data for data group in MDF4 file \"" + fileName + "\". Skipping.\n");
				continue;
			}
		}

		for (auto& dataset : datasets)
		{
			//MDF4Dataset& datasetRef = dataset.first;
			mdf::ChannelObserverPtr& obs = dataset.second;
			uint64_t nofSamples = obs->NofSamples();

			std::vector<double> channelValues;
			channelValues.reserve(nofSamples);

			double channelValue = 0.0; // Channel value (no scaling)
			double channelValueScaled = 0.0; // Channel value (scaled)
			for (size_t sample = 0; sample < obs->NofSamples(); ++sample)
			{
				const auto channelValid = obs->GetChannelValue(sample, channelValue);
				//const auto channelValidScaled = obs->GetEngValue(sample, channelValueScaled);

				if (channelValid)
				{
					channelValues.push_back(channelValue);
				}
				/*else if (channelValidScaled)
				{
					channelValues.push_back(channelValueScaled);
				}
				*/
				else
				{
					//this->logWarning("Failed to read sample " + std::to_string(sample) + " for channel \"" + datasetRef.getName() + "\" in MDF4 file \"" + fileName + "\". Skipping sample.\n");
				}
				
			}

			channelValues.shrink_to_fit();
			//datasetRef.setSamples(std::move(channelValues));

			
			//OT_LOG_TS("Dataset created { \"Name\": \"" << datasetRef.getName() << "\", \"Samples\": " << datasetRef.getSamplesSize() << " }");
		}

		dataGroup->ClearData();
	}

	return true;
}

void ot::MDF4Parser::initializeLogging()
{
	static bool g_isLoggingInitialized = false;
	if (g_isLoggingInitialized)
	{
		return;
	}

	mdf::MdfFactory::SetLogFunction2(
		[](mdf::MdfLogSeverity _severity, const std::string& _function, const std::string& _text)
		{
			LogFlag logMode = LogFlag::ERROR_LOG;

			switch (_severity)
			{
			case mdf::MdfLogSeverity::kTrace: OT_FALLTHROUGH
			case mdf::MdfLogSeverity::kDebug: OT_FALLTHROUGH
			case mdf::MdfLogSeverity::kInfo: OT_FALLTHROUGH
			case mdf::MdfLogSeverity::kNotice:
				MDF4Parser::log(_text);
				break;

			case mdf::MdfLogSeverity::kWarning:
				MDF4Parser::logWarning(_text);
				break;

			case mdf::MdfLogSeverity::kError: OT_FALLTHROUGH
			case mdf::MdfLogSeverity::kCritical: OT_FALLTHROUGH
			case mdf::MdfLogSeverity::kAlert: OT_FALLTHROUGH
			case mdf::MdfLogSeverity::kEmergency:
				MDF4Parser::logError(_text);
				break;

			default:
				OTAssert(0, "Unknown log severity level");
				OT_LOG_E("Unknown severity log serverity level");
				break;
			}
		}
	);

	g_isLoggingInitialized = true;
}
