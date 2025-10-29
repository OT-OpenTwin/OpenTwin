// @otlicense

#include "FixturePythonAPI.h"


TEST_F(FixturePythonAPI, TestModuleEntryPoint_SingleFunctionInModule)
{
	std::string expectedResult = "HelloWorld";
	std::string script = "import OpenTwin\n"
		"def HelloWorld():\n"
		"\tprint(\"Hello World\")\n"
		;
	SetupModule(script);
	std::string entryPoint = GetModuleEntryPoint();
	EXPECT_EQ(entryPoint, expectedResult);
}
TEST_F(FixturePythonAPI, TestModuleEntryPoint_MultipleFunctionsInModule)
{
	std::string expectedResult = "__main__";
	std::string script = "def HelloWorld():\n"
		"\tprint(\"Hello World\")\n"
		"def Kekse():\n"
		"\tprint(\"NomNom ... Nom\")\n"
		"def Truth():\n"
		"\tprint(\"42\")\n"
		;
	SetupModule(script);
	std::string entryPoint = GetModuleEntryPoint();
	EXPECT_EQ(entryPoint, expectedResult);
}

TEST_F(FixturePythonAPI, CreateParameterSet_String)
{
	const std::string expectedValue = "First";
	std::list<ot::Variable> parameter;
	parameter.emplace_back(ot::Variable(expectedValue.c_str()));
	parameter.emplace_back(ot::Variable(2));
	parameter.emplace_back(ot::Variable(3.));

	CPythonObjectNew pyParameter =CreateParameterSet(parameter); 
	PythonObjectBuilder pyObBuilder;
	std::string result = pyObBuilder.getStringValueFromTuple(pyParameter, 0, "First parameter");
	EXPECT_EQ(result, expectedValue);

}

TEST_F(FixturePythonAPI, CreateParameterSet_Int)
{
	const int32_t expectedValue = 2;
	std::list<ot::Variable> parameter;
	parameter.emplace_back(ot::Variable("First"));
	parameter.emplace_back(ot::Variable(expectedValue));
	parameter.emplace_back(ot::Variable(3.));

	CPythonObjectNew pyParameter = CreateParameterSet(parameter);
	PythonObjectBuilder pyObBuilder;
	int32_t result = pyObBuilder.getInt32ValueFromTuple(pyParameter, 1, "First parameter");
	EXPECT_EQ(result, expectedValue);

}

TEST_F(FixturePythonAPI, CreateParameterSet_Double)
{
	const double expectedValue = 3.;
	std::list<ot::Variable> parameter;
	parameter.emplace_back(ot::Variable("First"));
	parameter.emplace_back(ot::Variable(2));
	parameter.emplace_back(ot::Variable(expectedValue));

	CPythonObjectNew pyParameter = CreateParameterSet(parameter);
	PythonObjectBuilder pyObBuilder;
	double result = pyObBuilder.getDoubleValueFromTuple(pyParameter, 2, "First parameter");
	EXPECT_EQ(result, expectedValue);

}

TEST_F(FixturePythonAPI, ExecuteCommand)
{
	const std::string command = "file1 = open(r\"MyFile.txt\", \"w\")\n"
		"file1.write(\"Hello World\")\n"
		"file1.close()\n";

	ot::ReturnMessage msg =	ExecuteCommand(command);
	ASSERT_EQ(msg.getStatus(), ot::ReturnMessage::Ok);

}
