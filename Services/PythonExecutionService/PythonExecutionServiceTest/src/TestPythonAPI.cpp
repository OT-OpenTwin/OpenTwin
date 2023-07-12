#include "FixturePythonAPI.h"


TEST_F(FixturePythonAPI, TestModuleEntryPoint_SingleFunctionInModule)
{
	std::string expectedResult = "HelloWorld";
	std::string script = "def HelloWorld():"
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
	std::list<variable_t> parameter;
	parameter.emplace_back(variable_t(expectedValue.c_str()));
	parameter.emplace_back(variable_t(2));
	parameter.emplace_back(variable_t(3.));

	CPythonObjectNew pyParameter =CreateParameterSet(parameter); 
	PythonObjectBuilder pyObBuilder;
	std::string result = pyObBuilder.getStringValueFromTuple(pyParameter, 0, "First parameter");
	EXPECT_EQ(result, expectedValue);

}

TEST_F(FixturePythonAPI, CreateParameterSet_Int)
{
	const int32_t expectedValue = 2;
	std::list<variable_t> parameter;
	parameter.emplace_back(variable_t("First"));
	parameter.emplace_back(variable_t(expectedValue));
	parameter.emplace_back(variable_t(3.));

	CPythonObjectNew pyParameter = CreateParameterSet(parameter);
	PythonObjectBuilder pyObBuilder;
	int32_t result = pyObBuilder.getInt32ValueFromTuple(pyParameter, 1, "First parameter");
	EXPECT_EQ(result, expectedValue);

}

TEST_F(FixturePythonAPI, CreateParameterSet_Double)
{
	const double expectedValue = 3.;
	std::list<variable_t> parameter;
	parameter.emplace_back(variable_t("First"));
	parameter.emplace_back(variable_t(2));
	parameter.emplace_back(variable_t(expectedValue));

	CPythonObjectNew pyParameter = CreateParameterSet(parameter);
	PythonObjectBuilder pyObBuilder;
	double result = pyObBuilder.getDoubleValueFromTuple(pyParameter, 2, "First parameter");
	EXPECT_EQ(result, expectedValue);

}
