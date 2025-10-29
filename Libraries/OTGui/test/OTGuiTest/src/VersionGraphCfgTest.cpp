// @otlicense

#include "gtest/gtest.h"

// OpenTwin header
#include "OTGui/VersionGraphCfg.h"

namespace ot {
	namespace intern {
		namespace test {
			std::string linesToString(const std::list<std::string>& _lines, const std::string& _linePrefix) {
				std::string result;
				bool first = false;
				for (const std::string& line : _lines) {
					if (first) {
						result.append(_linePrefix + line);
					}
					else {
						result.append("\n" + _linePrefix + line);
					}
				}

				return result;
			}

			void testBranchesOrder(const ot::VersionGraphCfg::BranchesList& _branches, const std::list<std::string>& _expectedBranches) {
				// Check if size matches
				if (_branches.size() != _expectedBranches.size()) {
					FAIL() << "Unexpected number of branches { \"BranchesCount\": " + std::to_string(_branches.size()) +
						", \"ExpectedCount\": " + std::to_string(_expectedBranches.size()) + " }";
					return;
				}

				std::list<std::string> successBranchesList;

				// Go trough
				auto branchesIt = _branches.begin();
				auto expectedIt = _expectedBranches.begin();
				for (size_t ix = 0; branchesIt != _branches.end() && expectedIt != _expectedBranches.end(); branchesIt++, expectedIt++, ix++) {
					if (branchesIt->empty()) {
						FAIL() << "History of branches:\n" +
							linesToString(successBranchesList, "\t") +
							"\n\t(" + std::to_string(ix) + ") Empty branch provided!";
						return;
					}
					if (branchesIt->front().getBranchName() != *expectedIt) {
						FAIL() << "History of branches:\n" +
							linesToString(successBranchesList, "\t") +
							"\n\t(" + std::to_string(ix) + ") Unexpected branch { \"Branch\": \"" + branchesIt->front().getBranchName() +
							"\", \"ExpectedBranch\": \"" + *expectedIt + "\" }";;
						return;
					}
					else {
						successBranchesList.push_back("(" + std::to_string(ix) + ") Success.");
					}
				}

			}

			void testBranchesEqual(const ot::VersionGraphCfg::BranchesList& _branches, const std::list<std::list<std::string>>& _expectedVersions) {
				// Check if size matches
				if (_branches.size() != _expectedVersions.size()) {
					FAIL() << "Unexpected number of branches { \"BranchesCount\": " + std::to_string(_branches.size()) +
						", \"ExpectedCount\": " + std::to_string(_expectedVersions.size()) + " }";
					return;
				}

				auto branchesIt = _branches.begin();
				auto expectedIt = _expectedVersions.begin();

				std::list<std::string> successBranchesList;

				for (size_t ix = 0; branchesIt != _branches.end() && expectedIt != _expectedVersions.end(); branchesIt++, expectedIt++, ix++) {
					std::string successVersionsList;

					if (branchesIt->empty()) {
						FAIL() << "History of branches:\n" +
							linesToString(successBranchesList, "\t") +
							"\n\t(" + std::to_string(ix) + ") Empty branch";
						return;
					}

					// Check version count matches
					if (branchesIt->size() != expectedIt->size()) {
						FAIL() << "History of branches:\n" +
							linesToString(successBranchesList, "\t") +
							"\n\t(" + std::to_string(ix) + ") Unexpected number of versions in branch \"" + branchesIt->front().getBranchName() + "\": { \"VersionsCount\": " + std::to_string(branchesIt->size()) +
							", \"ExpectedCount\": " + std::to_string(expectedIt->size()) + " }";
						return;
					}

					auto versionsIt = branchesIt->begin();
					auto expectedVersionsIt = expectedIt->begin();
					
					for (size_t versionIx = 0; versionsIt != branchesIt->end() && expectedVersionsIt != expectedIt->end(); versionsIt++, expectedVersionsIt++, versionIx++) {
						if (versionsIt->getName() != *expectedVersionsIt) {
							FAIL() << "History of branches:\n" +
								linesToString(successBranchesList, "\t") +
								"\n\t(" + std::to_string(ix) + ") " + successVersionsList + ": Unexpected version (" + std::to_string(versionIx) + ") in branch \"" + branchesIt->front().getBranchName() + "\": \"" + branchesIt->front().getBranchName() + "\": { \"Version\": \"" + versionsIt->getName() +
								"\", \"ExpectedVersion\": \"" + *expectedVersionsIt + "\" }";
							return;
						}
						if (!successVersionsList.empty()) {
							successVersionsList.append(" --> ");
						}
						successVersionsList.append(versionsIt->getName());
					}

					successBranchesList.push_back("(" + std::to_string(ix) + ") " + successVersionsList);
				}
			}

			//! @brief Create default branches:
			//! Index   Name      Versions
			//! 0                 1 - 10
			//! 1       7.1       7.1.1 - 7.1.3
			//! 2       3.1       3.1.1 - 3.1.5
			//! 3       3.1.3.1   3.1.3.1.1 - 3.1.3.1.6
			//! 4       1.1       1.1.1 - 1.1.2
			//! 5       1.2       1.2.1 - 1.2.9
			void fillTestVersionGraph(ot::VersionGraphCfg& _graph) {
				ot::intern::test::testBranchesOrder(_graph.getBranches(), std::list<std::string>({}));

				// 3.1.1 - 3.1.5
				{
					ot::VersionGraphCfg::VersionsList branch;
					for (int i = 1; i <= 5; i++) {
						ot::VersionGraphVersionCfg newVersion("3.1." + std::to_string(i));
						branch.push_back(std::move(newVersion));
					}
					OT_UNUSED(_graph.insertBranch(std::move(branch)));
				}

				ot::intern::test::testBranchesOrder(_graph.getBranches(), std::list<std::string>({ "3.1" }));
				ot::intern::test::testBranchesEqual(_graph.getBranches(), std::list<std::list<std::string>>({
					std::list<std::string>({"3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" })
					}));

				// 1 - 10
				{
					ot::VersionGraphCfg::VersionsList branch;
					for (int i = 1; i <= 10; i++) {
						ot::VersionGraphVersionCfg newVersion(std::to_string(i));
						branch.push_back(std::move(newVersion));
					}
					OT_UNUSED(_graph.insertBranch(std::move(branch)));
				}

				ot::intern::test::testBranchesOrder(_graph.getBranches(), std::list<std::string>({ "", "3.1" }));
				ot::intern::test::testBranchesEqual(_graph.getBranches(), std::list<std::list<std::string>>({
					std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
					std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" })
					}));

				// 7.1.1 - 7.1.3
				{
					ot::VersionGraphCfg::VersionsList branch;
					for (int i = 1; i <= 3; i++) {
						ot::VersionGraphVersionCfg newVersion("7.1." + std::to_string(i));
						branch.push_back(std::move(newVersion));
					}
					OT_UNUSED(_graph.insertBranch(std::move(branch)));
				}

				ot::intern::test::testBranchesOrder(_graph.getBranches(), std::list<std::string>({ "", "7.1", "3.1" }));
				ot::intern::test::testBranchesEqual(_graph.getBranches(), std::list<std::list<std::string>>({
					std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
					std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
					std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" })
					}));

				// 1.2.1 - 1.2.9
				{
					ot::VersionGraphCfg::VersionsList branch;
					for (int i = 1; i <= 9; i++) {
						ot::VersionGraphVersionCfg newVersion("1.2." + std::to_string(i));
						branch.push_back(std::move(newVersion));
					}
					OT_UNUSED(_graph.insertBranch(std::move(branch)));
				}

				ot::intern::test::testBranchesOrder(_graph.getBranches(), std::list<std::string>({ "", "7.1", "3.1", "1.2" }));
				ot::intern::test::testBranchesEqual(_graph.getBranches(), std::list<std::list<std::string>>({
					std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
					std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
					std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
					std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" })
					}));

				// 3.1.3.1.1 - 3.1.3.1.6
				{
					ot::VersionGraphCfg::VersionsList branch;
					for (int i = 1; i <= 6; i++) {
						ot::VersionGraphVersionCfg newVersion("3.1.3.1." + std::to_string(i));
						branch.push_back(std::move(newVersion));
					}
					OT_UNUSED(_graph.insertBranch(std::move(branch)));
				}

				ot::intern::test::testBranchesOrder(_graph.getBranches(), std::list<std::string>({ "", "7.1", "3.1", "3.1.3.1", "1.2" }));
				ot::intern::test::testBranchesEqual(_graph.getBranches(), std::list<std::list<std::string>>({
					std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
					std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
					std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
					std::list<std::string>({                               "3.1.3.1.1", "3.1.3.1.2", "3.1.3.1.3", "3.1.3.1.4", "3.1.3.1.5", "3.1.3.1.6" }),
					std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" })
					}));

				// 1.1.1 - 1.1.2
				{
					ot::VersionGraphCfg::VersionsList branch;
					for (int i = 1; i <= 2; i++) {
						ot::VersionGraphVersionCfg newVersion("1.1." + std::to_string(i));
						branch.push_back(std::move(newVersion));
					}
					OT_UNUSED(_graph.insertBranch(std::move(branch)));
				}

				ot::intern::test::testBranchesOrder(_graph.getBranches(), std::list<std::string>({ "", "7.1", "3.1", "3.1.3.1", "1.1", "1.2" }));
				ot::intern::test::testBranchesEqual(_graph.getBranches(), std::list<std::list<std::string>>({
					std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
					std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
					std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
					std::list<std::string>({                               "3.1.3.1.1", "3.1.3.1.2", "3.1.3.1.3", "3.1.3.1.4", "3.1.3.1.5", "3.1.3.1.6" }),
					std::list<std::string>({  "1.1.1", "1.1.2" }),
					std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" })
					}));
			}
		}
	}
}

TEST(VersionGraphVersionCfgTest, InformationGathering) {
	ot::VersionGraphVersionCfg v1("1.2.2.3.3");
	EXPECT_TRUE(v1.getBranchName() == "1.2.2.3");
	EXPECT_TRUE(v1.getBranchNodeName() == "1.2.2");

	ot::VersionGraphVersionCfg v2("1.2.2");
	EXPECT_TRUE(v2 == v1.getBranchNodeName());

	ot::VersionGraphVersionCfg v3;
	v3 = v2.getBranchNodeName();
	EXPECT_TRUE(v3 == "1");
	EXPECT_TRUE(v3.getBranchName() == "");
	EXPECT_TRUE(v3.getBranchNodeName() == "");
}

TEST(VersionGraphVersionCfgTest, SerializationDeserialization) {
	ot::VersionGraphVersionCfg exportVersion("1.2.2.3.4", "label text", "description");
	exportVersion.setParentVersion("1.2.2.3.3");

	ot::JsonDocument exportDoc;
	exportVersion.addToJsonObject(exportDoc, exportDoc.GetAllocator());

	std::string json = exportDoc.toJson();
	EXPECT_FALSE(json.empty());

	ot::JsonDocument importDoc;
	EXPECT_TRUE(importDoc.fromJson(json));
	EXPECT_TRUE(importDoc.IsObject());

	ot::VersionGraphVersionCfg importVersion;
	ot::ConstJsonObject obj = importDoc.getConstObject();
	EXPECT_NO_THROW(importVersion.setFromJsonObject(obj));

	EXPECT_TRUE(exportVersion.getName() == importVersion.getName());
	EXPECT_TRUE(exportVersion.getLabel() == importVersion.getLabel());
	EXPECT_TRUE(exportVersion.getDescription() == importVersion.getDescription());
	EXPECT_TRUE(exportVersion.getParentVersion() == importVersion.getParentVersion());
}

TEST(VersionGraphCfgTest, Setup) {
	// Incrementing empty version
	EXPECT_TRUE(ot::VersionGraphCfg::incrementVersion("").empty());

	ot::VersionGraphCfg graph;
	graph.setActiveBranchName("1.2");
	graph.setActiveVersionName("1.2.2");

	// Test branches
	ot::intern::test::fillTestVersionGraph(graph);
	ot::intern::test::testBranchesOrder(graph.getBranches(), std::list<std::string>({ "", "7.1", "3.1", "3.1.3.1", "1.1", "1.2" }));
	ot::intern::test::testBranchesEqual(graph.getBranches(), std::list<std::list<std::string>>({
			std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
			std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
			std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
			std::list<std::string>({                               "3.1.3.1.1", "3.1.3.1.2", "3.1.3.1.3", "3.1.3.1.4", "3.1.3.1.5", "3.1.3.1.6" }),
			std::list<std::string>({  "1.1.1", "1.1.2" }),
			std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" })
		}));

	// Test adding additional versions
	{
		ot::VersionGraphVersionCfg newVersion("3.2.1");
		graph.insertVersion(std::move(newVersion));
		ot::intern::test::testBranchesOrder(graph.getBranches(), std::list<std::string>({ "", "7.1", "3.1", "3.1.3.1", "3.2", "1.1", "1.2" }));
		ot::intern::test::testBranchesEqual(graph.getBranches(), std::list<std::list<std::string>>({
			std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
			std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
			std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
			std::list<std::string>({                               "3.1.3.1.1", "3.1.3.1.2", "3.1.3.1.3", "3.1.3.1.4", "3.1.3.1.5", "3.1.3.1.6" }),
			std::list<std::string>({            "3.2.1" }),
			std::list<std::string>({  "1.1.1", "1.1.2" }),
			std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" })
			}));
	}
	{
		ot::VersionGraphVersionCfg newVersion("3.2.2");
		graph.insertVersion(std::move(newVersion));
		ot::intern::test::testBranchesOrder(graph.getBranches(), std::list<std::string>({ "", "7.1", "3.1", "3.1.3.1", "3.2", "1.1", "1.2" }));
		ot::intern::test::testBranchesEqual(graph.getBranches(), std::list<std::list<std::string>>({
			std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
			std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
			std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
			std::list<std::string>({                               "3.1.3.1.1", "3.1.3.1.2", "3.1.3.1.3", "3.1.3.1.4", "3.1.3.1.5", "3.1.3.1.6" }),
			std::list<std::string>({            "3.2.1", "3.2.2" }),
			std::list<std::string>({  "1.1.1", "1.1.2" }),
			std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" })
			}));
	}
	{
		ot::VersionGraphVersionCfg newVersion("7.2.2");
		graph.insertVersion(std::move(newVersion));
		ot::intern::test::testBranchesOrder(graph.getBranches(), std::list<std::string>({ "", "7.1", "7.2", "3.1", "3.1.3.1", "3.2", "1.1", "1.2" }));
		ot::intern::test::testBranchesEqual(graph.getBranches(), std::list<std::list<std::string>>({
			std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
			std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
			std::list<std::string>({                                "7.2.2" }),
			std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
			std::list<std::string>({                               "3.1.3.1.1", "3.1.3.1.2", "3.1.3.1.3", "3.1.3.1.4", "3.1.3.1.5", "3.1.3.1.6" }),
			std::list<std::string>({            "3.2.1", "3.2.2" }),
			std::list<std::string>({  "1.1.1", "1.1.2" }),
			std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" })
			}));
	}
	{
		ot::VersionGraphVersionCfg newVersion("3.1.3.1.1.2.2");
		graph.insertVersion(std::move(newVersion));
		ot::intern::test::testBranchesOrder(graph.getBranches(), std::list<std::string>({ "", "7.1", "7.2", "3.1", "3.1.3.1", "3.1.3.1.1.2", "3.2", "1.1", "1.2"}));
		ot::intern::test::testBranchesEqual(graph.getBranches(), std::list<std::list<std::string>>({
			std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
			std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
			std::list<std::string>({                                "7.2.2" }),
			std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
			std::list<std::string>({                               "3.1.3.1.1", "3.1.3.1.2", "3.1.3.1.3", "3.1.3.1.4", "3.1.3.1.5", "3.1.3.1.6" }),
			std::list<std::string>({                                 "3.1.3.1.1.2.2" }),
			std::list<std::string>({            "3.2.1", "3.2.2" }),
			std::list<std::string>({  "1.1.1", "1.1.2" }),
			std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" })
			}));
	}
	{
		ot::VersionGraphVersionCfg newVersion("3.1.3.1.1.1.1");
		graph.insertVersion(std::move(newVersion));
		ot::intern::test::testBranchesOrder(graph.getBranches(), std::list<std::string>({ "", "7.1", "7.2", "3.1", "3.1.3.1", "3.1.3.1.1.1", "3.1.3.1.1.2", "3.2", "1.1", "1.2" }));
		ot::intern::test::testBranchesEqual(graph.getBranches(), std::list<std::list<std::string>>({
			std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
			std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
			std::list<std::string>({                                "7.2.2" }),
			std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
			std::list<std::string>({                               "3.1.3.1.1", "3.1.3.1.2", "3.1.3.1.3", "3.1.3.1.4", "3.1.3.1.5", "3.1.3.1.6" }),
			std::list<std::string>({                                 "3.1.3.1.1.1.1" }),
			std::list<std::string>({                                 "3.1.3.1.1.2.2" }),
			std::list<std::string>({            "3.2.1", "3.2.2" }),
			std::list<std::string>({  "1.1.1", "1.1.2" }),
			std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" })
			}));
	}
	{
		ot::VersionGraphVersionCfg newVersion("3.1.3.1.1.2.1");
		graph.insertVersion(std::move(newVersion));
		ot::intern::test::testBranchesOrder(graph.getBranches(), std::list<std::string>({ "", "7.1", "7.2", "3.1", "3.1.3.1", "3.1.3.1.1.1", "3.1.3.1.1.2", "3.2", "1.1", "1.2" }));
		ot::intern::test::testBranchesEqual(graph.getBranches(), std::list<std::list<std::string>>({
			std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
			std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
			std::list<std::string>({                                "7.2.2" }),
			std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
			std::list<std::string>({                               "3.1.3.1.1", "3.1.3.1.2", "3.1.3.1.3", "3.1.3.1.4", "3.1.3.1.5", "3.1.3.1.6" }),
			std::list<std::string>({                                 "3.1.3.1.1.1.1" }),
			std::list<std::string>({                                 "3.1.3.1.1.2.1", "3.1.3.1.1.2.2" }),
			std::list<std::string>({            "3.2.1", "3.2.2" }),
			std::list<std::string>({  "1.1.1", "1.1.2" }),
			std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" })
			}));
	}
	{
		ot::VersionGraphVersionCfg newVersion("8.2.1");
		graph.insertVersion(std::move(newVersion));
		ot::intern::test::testBranchesOrder(graph.getBranches(), std::list<std::string>({ "", "8.2", "7.1", "7.2", "3.1", "3.1.3.1", "3.1.3.1.1.1", "3.1.3.1.1.2", "3.2", "1.1", "1.2" }));
		ot::intern::test::testBranchesEqual(graph.getBranches(), std::list<std::list<std::string>>({
			std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
			std::list<std::string>({                                     "8.2.1" }),
			std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
			std::list<std::string>({                                "7.2.2" }),
			std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
			std::list<std::string>({                               "3.1.3.1.1", "3.1.3.1.2", "3.1.3.1.3", "3.1.3.1.4", "3.1.3.1.5", "3.1.3.1.6" }),
			std::list<std::string>({                                 "3.1.3.1.1.1.1" }),
			std::list<std::string>({                                 "3.1.3.1.1.2.1", "3.1.3.1.1.2.2" }),
			std::list<std::string>({            "3.2.1", "3.2.2" }),
			std::list<std::string>({  "1.1.1", "1.1.2" }),
			std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" })
			}));
	}
	{
		ot::VersionGraphVersionCfg newVersion("8.1.1");
		graph.insertVersion(std::move(newVersion));
		ot::intern::test::testBranchesOrder(graph.getBranches(), std::list<std::string>({ "", "8.1", "8.2", "7.1", "7.2", "3.1", "3.1.3.1", "3.1.3.1.1.1", "3.1.3.1.1.2", "3.2", "1.1", "1.2" }));
		ot::intern::test::testBranchesEqual(graph.getBranches(), std::list<std::list<std::string>>({
			std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
			std::list<std::string>({                                     "8.1.1" }),
			std::list<std::string>({                                     "8.2.1" }),
			std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
			std::list<std::string>({                                "7.2.2" }),
			std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
			std::list<std::string>({                               "3.1.3.1.1", "3.1.3.1.2", "3.1.3.1.3", "3.1.3.1.4", "3.1.3.1.5", "3.1.3.1.6" }),
			std::list<std::string>({                                 "3.1.3.1.1.1.1" }),
			std::list<std::string>({                                 "3.1.3.1.1.2.1", "3.1.3.1.1.2.2" }),
			std::list<std::string>({            "3.2.1", "3.2.2" }),
			std::list<std::string>({  "1.1.1", "1.1.2" }),
			std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" })
			}));
	}
	{
		ot::VersionGraphVersionCfg newVersion("8.3.1");
		graph.insertVersion(std::move(newVersion));
		ot::intern::test::testBranchesOrder(graph.getBranches(), std::list<std::string>({ "", "8.1", "8.2", "8.3", "7.1", "7.2", "3.1", "3.1.3.1", "3.1.3.1.1.1", "3.1.3.1.1.2", "3.2", "1.1", "1.2" }));
		ot::intern::test::testBranchesEqual(graph.getBranches(), std::list<std::list<std::string>>({
			std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
			std::list<std::string>({                                     "8.1.1" }),
			std::list<std::string>({                                     "8.2.1" }),
			std::list<std::string>({                                     "8.3.1" }),
			std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
			std::list<std::string>({                                "7.2.2" }),
			std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
			std::list<std::string>({                               "3.1.3.1.1", "3.1.3.1.2", "3.1.3.1.3", "3.1.3.1.4", "3.1.3.1.5", "3.1.3.1.6" }),
			std::list<std::string>({                                 "3.1.3.1.1.1.1" }),
			std::list<std::string>({                                 "3.1.3.1.1.2.1", "3.1.3.1.1.2.2" }),
			std::list<std::string>({            "3.2.1", "3.2.2" }),
			std::list<std::string>({  "1.1.1", "1.1.2" }),
			std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" })
			}));
	}
	{
		ot::VersionGraphVersionCfg newVersion("8.3.1");
		graph.insertVersion(std::move(newVersion));
		ot::intern::test::testBranchesOrder(graph.getBranches(), std::list<std::string>({ "", "8.1", "8.2", "8.3", "7.1", "7.2", "3.1", "3.1.3.1", "3.1.3.1.1.1", "3.1.3.1.1.2", "3.2", "1.1", "1.2" }));
		ot::intern::test::testBranchesEqual(graph.getBranches(), std::list<std::list<std::string>>({
			std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
			std::list<std::string>({                                     "8.1.1" }),
			std::list<std::string>({                                     "8.2.1" }),
			std::list<std::string>({                                     "8.3.1" }),
			std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
			std::list<std::string>({                                "7.2.2" }),
			std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
			std::list<std::string>({                               "3.1.3.1.1", "3.1.3.1.2", "3.1.3.1.3", "3.1.3.1.4", "3.1.3.1.5", "3.1.3.1.6" }),
			std::list<std::string>({                                 "3.1.3.1.1.1.1" }),
			std::list<std::string>({                                 "3.1.3.1.1.2.1", "3.1.3.1.1.2.2" }),
			std::list<std::string>({            "3.2.1", "3.2.2" }),
			std::list<std::string>({  "1.1.1", "1.1.2" }),
			std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" })
			}));
	}
	{
		ot::VersionGraphVersionCfg newVersion("1.2.1.1.1");
		graph.insertVersion(std::move(newVersion));
		ot::intern::test::testBranchesOrder(graph.getBranches(), std::list<std::string>({ "", "8.1", "8.2", "8.3", "7.1", "7.2", "3.1", "3.1.3.1", "3.1.3.1.1.1", "3.1.3.1.1.2", "3.2", "1.1", "1.2", "1.2.1.1" }));
		ot::intern::test::testBranchesEqual(graph.getBranches(), std::list<std::list<std::string>>({
			std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
			std::list<std::string>({                                     "8.1.1" }),
			std::list<std::string>({                                     "8.2.1" }),
			std::list<std::string>({                                     "8.3.1" }),
			std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
			std::list<std::string>({                                "7.2.2" }),
			std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
			std::list<std::string>({                               "3.1.3.1.1", "3.1.3.1.2", "3.1.3.1.3", "3.1.3.1.4", "3.1.3.1.5", "3.1.3.1.6" }),
			std::list<std::string>({                                 "3.1.3.1.1.1.1" }),
			std::list<std::string>({                                 "3.1.3.1.1.2.1", "3.1.3.1.1.2.2" }),
			std::list<std::string>({            "3.2.1", "3.2.2" }),
			std::list<std::string>({  "1.1.1", "1.1.2" }),
			std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" }),
			std::list<std::string>({    "1.2.1.1.1" })
			}));
	}
	{
		ot::VersionGraphVersionCfg newVersion("1.2.8.1.1");
		graph.insertVersion(std::move(newVersion));
		ot::intern::test::testBranchesOrder(graph.getBranches(), std::list<std::string>({ "", "8.1", "8.2", "8.3", "7.1", "7.2", "3.1", "3.1.3.1", "3.1.3.1.1.1", "3.1.3.1.1.2", "3.2", "1.1", "1.2", "1.2.8.1", "1.2.1.1" }));
		ot::intern::test::testBranchesEqual(graph.getBranches(), std::list<std::list<std::string>>({
			std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
			std::list<std::string>({                                     "8.1.1" }),
			std::list<std::string>({                                     "8.2.1" }),
			std::list<std::string>({                                     "8.3.1" }),
			std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
			std::list<std::string>({                                "7.2.2" }),
			std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
			std::list<std::string>({                               "3.1.3.1.1", "3.1.3.1.2", "3.1.3.1.3", "3.1.3.1.4", "3.1.3.1.5", "3.1.3.1.6" }),
			std::list<std::string>({                                 "3.1.3.1.1.1.1" }),
			std::list<std::string>({                                 "3.1.3.1.1.2.1", "3.1.3.1.1.2.2" }),
			std::list<std::string>({            "3.2.1", "3.2.2" }),
			std::list<std::string>({  "1.1.1", "1.1.2" }),
			std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" }),
			std::list<std::string>({                                                                   "1.2.8.1.1" }),
			std::list<std::string>({    "1.2.1.1.1" })
			}));
	}
	{
		ot::VersionGraphVersionCfg newVersion("1.2.3.1.1");
		graph.insertVersion(std::move(newVersion));
		ot::intern::test::testBranchesOrder(graph.getBranches(), std::list<std::string>({ "", "8.1", "8.2", "8.3", "7.1", "7.2", "3.1", "3.1.3.1", "3.1.3.1.1.1", "3.1.3.1.1.2", "3.2", "1.1", "1.2", "1.2.8.1", "1.2.3.1", "1.2.1.1" }));
		ot::intern::test::testBranchesEqual(graph.getBranches(), std::list<std::list<std::string>>({
			std::list<std::string>({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }),
			std::list<std::string>({                                     "8.1.1" }),
			std::list<std::string>({                                     "8.2.1" }),
			std::list<std::string>({                                     "8.3.1" }),
			std::list<std::string>({                                "7.1.1", "7.1.2", "7.1.3" }),
			std::list<std::string>({                                "7.2.2" }),
			std::list<std::string>({            "3.1.1", "3.1.2", "3.1.3", "3.1.4", "3.1.5" }),
			std::list<std::string>({                               "3.1.3.1.1", "3.1.3.1.2", "3.1.3.1.3", "3.1.3.1.4", "3.1.3.1.5", "3.1.3.1.6" }),
			std::list<std::string>({                                 "3.1.3.1.1.1.1" }),
			std::list<std::string>({                                 "3.1.3.1.1.2.1", "3.1.3.1.1.2.2" }),
			std::list<std::string>({            "3.2.1", "3.2.2" }),
			std::list<std::string>({  "1.1.1", "1.1.2" }),
			std::list<std::string>({  "1.2.1", "1.2.2", "1.2.3", "1.2.4", "1.2.5", "1.2.6", "1.2.7", "1.2.8", "1.2.9" }),
			std::list<std::string>({                                                                   "1.2.8.1.1" }),
			std::list<std::string>({                      "1.2.3.1.1" }),
			std::list<std::string>({    "1.2.1.1.1" })
			}));
	}
}