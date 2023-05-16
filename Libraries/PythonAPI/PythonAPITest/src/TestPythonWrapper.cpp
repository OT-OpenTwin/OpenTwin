#include <gtest/gtest.h>
#include "FixturePythonWrapper.h"
#include "PythonParameter.h"

TEST_F(FixturePythonWrapper, InitializationTestSuccess)
{
	ASSERT_NO_THROW(getPythonWrapper()->InitializePythonInterpreter());
}

TEST_F(FixturePythonWrapper, InitializationTestFailure)
{
	SetFalsePythonSysPath();
	ASSERT_DEATH(getPythonWrapper()->InitializePythonInterpreter(),""); //Problem in initialization causes an os signal (c-level) which cannot be treated with handlers on the level of this application, nor on python level
}