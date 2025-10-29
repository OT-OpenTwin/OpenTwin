// @otlicense

//! @file GlobalTestingFlags.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#ifndef OT_TESTING_GLOBAL_AllTestsEnabled

//! @def OT_TESTING_GLOBAL_DefaultTestEnabledState
//! @brief Default enabled state (true/false) used for the different tests.
#define OT_TESTING_GLOBAL_AllTestsEnabled false

#endif // !OT_TESTING_GLOBAL_AllTestsEnabled

#if OT_TESTING_GLOBAL_AllTestsEnabled==true

//! @def OT_TESTING_GLOBAL_RuntimeTestingEnabled
//! If enabled any runtime tests are enabled.
//! @warning When deploying check that the value is false or true is clearly wanted.
#define OT_TESTING_GLOBAL_RuntimeTestingEnabled OT_TESTING_GLOBAL_AllTestsEnabled

#else // OT_TESTING_GLOBAL_AllTestsEnabled==true

//! @def OT_TESTING_GLOBAL_RuntimeTestingEnabled
//! If enabled any runtime tests are enabled.
//! @warning When deploying check that the value is false or true is clearly wanted.
#define OT_TESTING_GLOBAL_RuntimeTestingEnabled false

#endif // OT_TESTING_GLOBAL_AllTestsEnabled==true